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

Value* TransactionManager::readData(Transaction* t, Key key) { //NULL in case of no key found, pointer to string otherwise
    Value* contains = t->alreadyInReadSet(key);
    if (contains == NULL) {
       contains = t->alreadyInWriteSet(key);
    }

    if (contains != NULL) {
        return contains;
    }

    Interval i = t->currentInterval;
    DataServerClient c = routingService.getConnection(key);
    
    ReadReply rR;

    //TODO lock
    c.handleReadRequest(rR, tid, i, key);
    //TODO unlock

    if (rR.OperationState == FAIL_NO_VERSION) {
        t->addToReadSet(ReadSetEntry(key, NULL, t->currentInterval, t->currentInterval, c));
        return NULL;
    }
   
    if (rR.OperationState == R_LOCK_SUCCESS) {
        t->currentInterval = rR.interval;
        t->addToReadSet(ReadSetEntry(key, rR.value, rR.interval, rR.potential));
        return rR.value; 
    }

    //TODO: what should I do if I receive an error status?
    abortTransaction(t);
    return NULL;
}

int TransactionManager::declareWrite(Transaction* t, Key key) {
    Value contains = t->alreadyInWriteSet(key);
    if (contains != NULL) {
        return 1; //nothing to be done, I already have a write lock for this key
    }

    Interval  i = t->currentInterval;
    DataServerClient c = routingService.getConnection(key);

    TimestampInterval ti; 
    //TODO lock
    c.handleHintRequest(ti, tid, i, key);
    //TODO unlock
    if ((ti.start == MIN_TIMESTAMP) && (ti.end == MIN_TIMESTAMP)) { //no interval found
        restartTransaction(t);
        return 0;
    }
    t->currentInterval = ti;
    t->addToHinSet(key, ti);
    return 1;
}

int TransactionManager::writeData(Transaction* t, Key key, Value value) {
    Value contains = t->alreadyInWriteSet(key); //not good enough if it's in the read set; need to take write lock for it
    if (contains != NULL) {
        updateValueLocally(tid, key, value);
        return;
    }
    Interval i = t->currentInterval;
    DataServerClient c = routingService.getConnection(key);
    WriteReply wR;
    //TODO lock
    c.handleWriteRequest(wR, tid, i, key, value);
    //TODO unlock

    if (wr.operationState == OperationState::W_LOCK_SUCCESS) {
        t->currentInterval = wr.interval; 
        t->addToWriteSet(WriteSetEntry(key, value, wR.interval, wR.potential));
        t->writeSetServers.insert(c);
        return 1;
    }
    if (wr.operationState == OperationState::FAIL_READ_MARK_LARGE) {
        restartTransaction(t, wr.potential.start);
        return 0;
    }

    if (wr.operationState == OperationState::FAIL_INTERSECTION_EMPTY) {
        restartTransaction(t);
        return 0;
    }

    abortTransaction(t); //other reason, should abort
    return 0;
}

int restartTransaction(Transaction* t, Timestamp startBound, Timestamp endBound) { //TODO what other information should I pass as parameters to help with a propper restart?
    Timespan duration = t->initialInterval.end - t->initialInterval.start;
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
    t->currentInterval = t->intialInterval;
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

    return 0;
}





