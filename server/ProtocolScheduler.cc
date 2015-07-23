#include "ProtocolScheduler.h"

ProtocolScheduler::ProtocolScheduler() {

}

ProtocolScheduler::~ProtocolScheduler() {
}

ClientReply* ProtocolScheduler::handleRead(TransactionId tid, TimestampInterval interval, Key k) {
    LockInfo* lockInfo = new LockInfo(); //TODO need a better solution; shouldn't have to allocate a new object for every request
    timer->start();
    Value* value = NULL;
    while(1) {
        tryReadLock(k,interval,lockInfo);
        if (lockInfo->state == R_LOCK_SUCCESS) {
            value = dataStore->read(toDsKey(k,v->timestamp));
            break;
        }
        if (timer->timeout()) {
            return new ClientReply(tid, TIMEOUT);
        }
        pause(PAUSE_LENGTH);
    }
    return new ClientReply(tid, READ_REPLY, lockInfo, value);
}

ClientReply* ProtocolScheduler::handleWrite(TransactionId tid, TimestampInterval interval, Key k, Value v) {
    Version* prev = getVersion(k,interval,OP_WRITE); 
    LockInfo* lockInfo = new LockInfo();
    tryWriteLock(k, interval, lockInfo);

    if (lockInfo->state != W_LOCK_SUCCESS) {
        abortTransaction(tid);
        return new ClientReply(tid, WRITE_REPLY, lockInfo);
    }
    
    if (pendingWriteSets.find(tid) == pendingWriteSets.end()){
        pendingWriteSets[tid] = new std::vector<Version*>;
    }

    pendingWriteSets[tid].insert(lockInfo->version, k, v); //TODO: do struct for this, containing a version, a value, and a lock
    return new ClientReply(tid, WRITE_REPLY, lockInfo);
}

ClientReply* ProtocolScheduler::handleCommit(TransactionId tid, Timestamp ts) {
    WriteSet* ws = pendingWriteSets(tid); 
    if (ws == NULL) {
        return new ClientReply(tid, WRITES_NOT_FOUND);
    }
    while (!ws->empty()) {
        WSEntry * ws_entry = ws->pop();
        ws_entry->version->timestamp = ts; 
        if (ws_entry->version->maxReadFrom < ts) {
            ws_entry->version->maxReadFrom = ts;
        }
        ws_entry->version->state = COMMITTED;
        versionManager->persist(ws_entry->version);
        dataStore.write(toDsKey(ws_entry->key,ts), ws_entry->value);
        delete(ws_entry);
        //TODO: delete members as well?
    }
    removePendingWriteSet(tid);
    return new ClientReply(tid, COMMIT_ACK);
}

ClientReply* ProtocolScheduler::handleAbort(TransactionId tid) {
    WriteSet* ws = pendingWriteSets(tid);
    if (ws == NULL) {
        return new ClientReply(tid, WRITES_NOT_FOUND);
    }
    while (!ws->empty()) {
        WSEntry * ws_entry = ws->pop();
        versionManager->removeVersion(we_entry->key, ws_entry->version);
        delete(ws_entry);
        //TODO: what else needs to be deleted? is the version deleted by removeVersion()?
    }
    removePendingWriteSet(tid);
    return new ClientReply(tid,ABORT_ACK);
}

ClientReply* ProtocolScheduler::handleHintRequest(TimestampInterval interval, Key k) {

}

//TODO add code handling transactions aborted by the recovery manager
