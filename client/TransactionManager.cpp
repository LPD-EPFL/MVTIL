#include "TransactionManager.h"


TransactionManager::TransactionManager(int64_t clientId) : id(clientId) {

}

TransactionManager::~TransactionManager() {

}

void TransactionManager::transactionStart(bool isReadOnly) {
    TransactionId tid = getNewTransactionId();
    ongoingTransactions.insert(Transaction(tid, isReadOnly), oracle.getInterval(isReadOnly));
    return;
}

int TransactionManager::transactionEnd() {

}

Value* TransactionManager::readData(Key key) {
    return "";
}

int TransactionManager::declareWrite(TransactionId tid, Key key) {
    Transaction* t = getTransaction(tid);
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
        restartTransaction(tid);
        return 0;
    }
    t->currentInterval = ti;
    t->addToHinSet(key, ti, c);
    return 1;
}

void writeData(TransactionId tid, Key key, Value value) {
    Transaction* t = getTransaction(tid);
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
        t->addToWriteSet(WriteSetEntry(key, value, wR.interval, wR.potential, c));
        return 1;
    }
    if (wr.operationState == OperationState::FAIL_READ_MARK_LARGE) {
        restartTransaction(tid, wr.potential.start);
        return 0;
    }

    if (wr.operationState == OperationState::FAIL_INTERSECTION_EMPTY) {
        restartTransaction(tid);
        return 0;
    }

    abortTransaction(tid); //other reason, should abort
    return 0;
}


