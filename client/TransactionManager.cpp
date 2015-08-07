/* 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "TransactionManager.h"


TransactionManager::TransactionManager(int64_t clientId) : id(clientId) {

}

TransactionManager::~TransactionManager() {

}

Transaction* TransactionManager::transactionStart(bool isReadOnly) {
    TransactionId tid = getNewTransactionId();
    Transaction* t = new Transaction(tid, isReadOnly, oracle.getInterval(isReadOnly));
    return;
}

int TransactionManager::transactionEnd() {

}

int TransactionManager::readData(Transaction* t, Key key, Value ** val) { //NULL in case of no key found, pointer to string otherwise
    Value* contains = t->alreadyInReadSet(key);
    if (contains == NULL) {
       contains = t->alreadyInWriteSet(key);
    }

    if (contains != NULL) {
        *val = contains;
        return 1;
    }

    Interval i = t->currentInterval;
    auto s = connectionService.getServer(key);
    
    ReadReply rR;

    s->lock();
    s->client.handleReadRequest(rR, tid, i, key, t->isReadOnly);
    s->unlock();

    if (rR.OperationState == FAIL_NO_VERSION) {
        Timestamp in = t->currentInterval;
        in.start = MIN_TIMESTAMP;
        t->addToReadSet(SetEntry(key, NULL, in, in, c));
        *val = NULL;
        return 1;
    }
   
    if (rR.OperationState == R_LOCK_SUCCESS) {
        t->currentInterval = rR.interval;
        t->addToReadSet(SetEntry(key, rR.value, rR.interval, rR.potential));
        auto ptr = new Value(rR.value);
        *val = ptr; 
        return 1;
    }
    
    //some error or timeout has occurred; should abort transaction to prevent deadlocks
    abortTransaction(t);
    *val = NULL;
    return 0;
}

int TransactionManager::declareWrite(Transaction* t, Key key) {
    Value contains = t->alreadyInWriteSet(key);
    if (contains != NULL) {
        return 1; //nothing to be done, I already have a write lock for this key
    }

    bool done = false;
    while (!done) {

        Interval  i = t->currentInterval;
        auto s = connectionService.getServer(key);

        HintReply hR; 
        s->lock();
        s->client.handleHintRequest(hR, tid, i, key);
        s->unlock();

        if (hR.state != OperationState::HINT_OK) { //no interval found
            if (restartTransaction(t, hR.potential.start, hR.potential.end) == 1) {
                continue;
            } else {
                return 0;
            }
        }
        done = true;
    }
    t->currentInterval = hR.interval;
    t->addToHintSet(SetEntry(key, NULL, hR.interval, hR.potential));
    return 1;
}

int TransactionManager::writeData(Transaction* t, Key key, Value value) {
    Value contains = t->alreadyInWriteSet(key); //not good enough if it's in the read set; need to take write lock for it
    if (contains != NULL) {
        updateValueLocally(tid, key, value);
        return;
    }

    while (1) {
        Interval i = t->currentInterval;
        auto s = connectionService.getServer(key);
        WriteReply wR;

        s->lock();
        s->client.handleWriteRequest(wR, tid, i, key, value);
        s->unlock();

        if (wr.operationState == OperationState::W_LOCK_SUCCESS) {
            t->currentInterval = wr.interval; 
            t->addToWriteSet(SetEntry(key, value, wR.interval, wR.potential));
            t->writeSetServers.insert(c);
            return 1;
        }
        if (wr.operationState == OperationState::FAIL_READ_MARK_LARGE) {
            if (restartTransaction(t, wr.potential.start, MIN_TIMESTAMP) == 1) {
                continue;
            } else {
                return 0;
            }
        }

        if (wr.operationState == OperationState::FAIL_INTERSECTION_EMPTY) {
            if (restartTransaction(t, wr.potential.start, wr.potential.end) == 1) {
                continue;
            } else {
                return 0;
            }
        }

        abortTransaction(t); //other reason, should abort
        return 0;
    }
}

int TransactionManager::restartTransaction(Transaction* t, Timestamp startBound, Timestamp endBound) {
    Timespan duration = t->initialInterval.end - t->initialInterval.start;
    if (t->numRestarts > RESTART_THRESHOLD) {
        abortTransaction(t, duration);
    }
    bool redo = false;
    duration *= INTERVAL_MULTIPLICATION_FACTOR;
    if (duration > INTERVAL_MAX_DURATION) {
        duration = INTERVAL_MAX_DURATION;
    }
    if (startBound > t->initialInterval.start) {
        t->initialInterval.start = startBound;
    } 
    t->initialInterval.end = t->initialInterval.start + duration;
    if (t->initialInterval.end < endBound) {
        if ((endBound - t->initialInterval.start) < INTERVAL_MAX_DURATION) {
         t->initialInterval.end = endBound;
        } else {
            redo = true;
        }
    }
    if (redo) {
        abortTransaction(t, duration);
        return 0;
    }

    //Initial interval should allow operation that caused the failure to complete successfully if things remain the same

    t->currentInterval = t->intialInterval;
    t->numRestarts++;

    /*
     * for all in read, write and hint set:
     * check in the "potential" fields if I can expand the intervals for all such that they will intersect with the new interval;
     * if no, basically trigger an abort
     * if yes, actually message the read and write sets and actually try to expand them (only entries that do not intersect t->initial interval already)
     * if at least one fails, message read and write sets again, and essentially "abort" the transaction (remove all data re transaction from the servers).
     */
    bool can_restart = true;

    for (auto& entry: t->readSet) {
        if (!intersects(t->initialInterval, entry->potential)) {
            can_restart = false;
        }
    }

    if (can_restart) {
        for (auto& entry: t->writeSet) {
             if (!intersects(t->initialInterval, entry->potential)) {
                 can_restart = false;
            }
        }
    }
    
    if (can_restart) {
        for (auto& entry: t->hintSet) {
             if (!intersects(t->initialInterval, entry->potential)) {
                 can_restart = false;
            }
        }
    }

    if (can_restart) {
        for (auto& entry: t->readSet) {
            if(!intersects(t->currentInterval, entry->interval)) {
                Interval newInterval = computeIntersection(entry->potential, t->currentInterval);
                auto s = connectionService.getServer(key);
                ExpandReadReply eR;

                s->lock();
                s->client.handleExpandReadRequest(eR, tid, newInterval, key);
                s->unlock();

                if (eR.state != EXPANSION_OK) {
                    can_restart = false;
                    break;
                }

                t->currentInterval = eR.interval;
                entry->interval = eR.interval;
                entry->potential = eR.potential;
            }
        }
        if (can_restart) {
            for (auto& entry: t->writeSet) {
                if(!intersects(t->currentInterval, entry->interval)) {
                    Interval newInterval = computeIntersection(entry->potential, t->currentInterval);
                    auto s = connectionService.getServer(key);
                    ExpandWwriteReply eW;

                    s->lock();
                    s->client.handleExpandWriteRequest(eW, tid, newInterval, key);
                    s->unlock();

                    if (eW.state != EXPANSION_OK) {
                        can_restart = false;
                        break;
                    }

                    t->currentInterval = eW.interval;
                    entry->interval = eW.interval;
                    entry->potential = eW.potential;
                }
            }
        }
    }

    if (!can_restart) {
        abortTransaction(t, duration);
        return 0;
    }

    return 1;
}

int abortTransaction(Transaction* t, Timespan duration) {
    // release all write locks 
    for (auto& value: t->writeSetServers) {
        c.send_handleAbort(t->transactionId);
    }
    AbortReply aR;
    for (auto& value: t->writeSetServers) {
        c.recv_handleAbort(&aR); //not really necessary to check the state
    }

    t->readSet.clear(); //TODO: should I keep the read set in the hope that at some point it might be useful?
    t->writeSet.clear();
    t->hintSet.clear();
    t->writeSetServers.clear();

    t->transactionId = getNewTransactionId();
    t->initialInterval = oracle.getInterval(t->isReadOnly, duration);
    t->currentInterval = t->initialInterval; 
    t->numRestarts = 0;

    return 0;
}





