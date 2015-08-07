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


TransactionManager::TransactionManager(int64_t clientId) : id(clientId), crt(1) {
}

TransactionManager::~TransactionManager() {

}

TransactionId TransactionManager::getNewTransactionId() {
    return (id + (MULTIPLICATION_FACTOR * crt++));
}

Transaction* TransactionManager::transactionStart(bool isReadOnly) {
    TransactionId tid = getNewTransactionId();
    Transaction* t = new Transaction(tid, isReadOnly, oracle.getInterval(isReadOnly, INITIAL_INTERVAL));
    return t;
}

int TransactionManager::transactionEnd(Transaction* t) {

    return 0;
}

int TransactionManager::readData(Transaction* t, Key key, Value ** val) { //NULL in case of no key found, pointer to string otherwise
    Value* contains = t->alreadyInWriteSet(key);
    if (contains == NULL) {
       contains = t->alreadyInReadSet(key);
    }

    if (contains != NULL) {
        *val = contains;
        return 1;
    }

    TimestampInterval i = t->currentInterval;
    auto s = communicationService.getServer(key);
    
    ReadReply rR;

    s->lock();
    s->client->handleReadRequest(rR, t->transactionId, i, key, t->isReadOnly);
    s->unlock();

    if (rR.state == OperationState::FAIL_NO_VERSION) {
        TimestampInterval in = t->currentInterval;
        in.start = MIN_TIMESTAMP;
        t->addToReadSet(key, SetEntry(NULL_VALUE, in, in));
        *val = NULL;
        return 1;
    }
   
    if (rR.state == OperationState::R_LOCK_SUCCESS) {
        t->currentInterval = rR.interval;
        t->addToReadSet(key, SetEntry(rR.value, rR.interval, rR.potential));
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
    Value* contains = t->alreadyInWriteSet(key);
    if (contains != NULL) {
        return 1; //nothing to be done, I already have a write lock for this key
    }

    HintReply hR; 
    bool done = false;
    while (!done) {

        TimestampInterval  i = t->currentInterval;
        auto s = communicationService.getServer(key);

        s->lock();
        s->client->handleHintRequest(hR, t->transactionId, i, key);
        s->unlock();

        if (hR.state != OperationState::HINT_OK) { //no interval found
            if (restartTransaction(t, hR.potential.start, hR.potential.finish) == 1) {
                continue;
            } else {
                return 0;
            }
        }
        done = true;
    }
    t->currentInterval = hR.interval;
    t->addToHintSet(key, SetEntry(NULL, hR.interval, hR.potential));
    return 1;
}

int TransactionManager::writeData(Transaction* t, Key key, Value value) {
    Value* contains = t->alreadyInWriteSet(key); //not good enough if it's in the read set; need to take write lock for it
    if (contains != NULL) {
        t->updateValue(key, value);
        return 1;
    }

    WriteReply wR;
    while (1) {
        TimestampInterval i = t->currentInterval;
        auto s = communicationService.getServer(key);

        s->lock();
        s->client->handleWriteRequest(wR, t->transactionId, i, key, value);
        s->unlock();

        if (wR.state == OperationState::W_LOCK_SUCCESS) {
            t->currentInterval = wR.interval; 
            t->addToWriteSet(key, SetEntry(value, wR.interval, wR.potential));
            t->writeSetServers.insert(s);
            return 1;
        }
        if (wR.state == OperationState::FAIL_READ_MARK_LARGE) {
            if (restartTransaction(t, wR.potential.start, MIN_TIMESTAMP) == 1) {
                continue;
            } else {
                return 0;
            }
        }

        if (wR.state == OperationState::FAIL_INTERSECTION_EMPTY) {
            if (restartTransaction(t, wR.potential.start, wR.potential.finish) == 1) {
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
    Timespan duration = t->initialInterval.finish - t->initialInterval.start;
    if (t->numRestarts > RESTART_THRESHOLD) {
        abortTransaction(t);
    }
    bool abort = false;
    duration *= INTERVAL_MULTIPLICATION_FACTOR;
    if (duration > INTERVAL_MAX_DURATION) {
        duration = INTERVAL_MAX_DURATION;
    }
    if (startBound > t->initialInterval.start) {
        t->initialInterval.start = startBound;
    } 
    t->initialInterval.finish = t->initialInterval.start + duration;
    if (t->initialInterval.finish < endBound) {
        if ((endBound - t->initialInterval.start) < INTERVAL_MAX_DURATION) {
         t->initialInterval.finish = endBound;
        } else {
            abort = true;
        }
    }
    if (abort) {
        abortTransaction(t);
        return 0;
    }

    //Initial interval should allow operation that caused the failure to complete successfully if things remain the same

    t->currentInterval = t->initialInterval;
    t->numRestarts++;

    /*
     * for all in read, write and hint set:
     * check in the "potential" fields if I can expand the intervals for all such that they will intersect with the new interval;
     * if no, basically trigger an abort
     * if yes, actually message the read and write sets and actually try to expand them (only entries that do not intersect t->initial interval already)
     * if at least one fails, message read and write sets again, and essentially "abort" the transaction (remove all data re transaction from the servers).
     */
    bool can_restart = true;

    for (auto entry: t->readSet) {
        if (!intersects(t->initialInterval, entry.second.potential)) {
            can_restart = false;
        }
    }

    if (can_restart) {
        for (auto entry: t->writeSet) {
             if (!intersects(t->initialInterval, entry.second.potential)) {
                 can_restart = false;
            }
        }
    }
    
    if (can_restart) {
        for (auto entry: t->hintSet) {
             if (!intersects(t->initialInterval, entry.second.potential)) {
                 can_restart = false;
            }
        }
    }

    if (can_restart) {
        for (auto entry: t->readSet) {
            if(!intersects(t->currentInterval, entry.second.interval)) {
                TimestampInterval newInterval = computeIntersection(entry.second.potential, t->currentInterval);
                auto s = communicationService.getServer(entry.first);
                ExpandReadReply eR;

                s->lock();
                s->client->handleExpandRead(eR, t->transactionId, entry.second.potential.start, newInterval, entry.first);
                s->unlock();

                if (eR.state != OperationState::EXPANSION_OK) {
                    can_restart = false;
                    break;
                }

                t->currentInterval = eR.interval;
                entry.second.interval = eR.interval;
                entry.second.potential = eR.potential;
            }
        }
        if (can_restart) {
            for (auto entry: t->writeSet) {
                if(!intersects(t->currentInterval, entry.second.interval)) {
                    TimestampInterval newInterval = computeIntersection(entry.second.potential, t->currentInterval);
                    auto s = communicationService.getServer(entry.first);
                    ExpandWriteReply eW;

                    s->lock();
                    s->client->handleExpandWrite(eW, t->transactionId, entry.second.interval.start,newInterval, entry.first);
                    s->unlock();

                    if (eW.state != OperationState::EXPANSION_OK) {
                        can_restart = false;
                        break;
                    }

                    t->currentInterval = eW.interval;
                    entry.second.interval = eW.interval;
                    entry.second.potential = eW.potential;
                }
            }
        }
    }

    if (!can_restart) {
        abortTransaction(t);
        return 0;
    }

    return 1;
}

int TransactionManager::abortTransaction(Transaction* t) {
    // release all write locks 
    for (auto& c: t->writeSetServers) {
        c->client->send_handleAbort(t->transactionId);
    }
    AbortReply aR;
    for (auto& c: t->writeSetServers) {
        c->client->recv_handleAbort(aR); //not really necessary to check the state
    }

    t->readSet.clear(); //TODO: should I keep the read set in the hope that at some point it might be useful?
    t->writeSet.clear();
    t->hintSet.clear();
    t->writeSetServers.clear();

    Timespan duration = t->initialInterval.finish  - t->initialInterval.start; //TODO when I abort, should I restart with the minimum duration or not?
    if ((duration * INTERVAL_MULTIPLICATION_FACTOR) < INTERVAL_MAX_DURATION) {
        duration *= INTERVAL_MULTIPLICATION_FACTOR;        
    } else {
        duration = INTERVAL_MAX_DURATION;
    }

    t->transactionId = getNewTransactionId();
    t->initialInterval = oracle.getInterval(t->isReadOnly, duration);
    t->currentInterval = t->initialInterval; 
    t->numRestarts = 0;

    return 0;
}





