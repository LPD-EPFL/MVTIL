#include "ProtocolScheduler.h"

ProtocolScheduler::ProtocolScheduler() {

}

ProtocolScheduler::~ProtocolScheduler() {
}

ClientReply* ProtocolScheduler::handleRead(TransactionId tid, TimestampInterval interval, Key k) {
    LockInfo* lockInfo;
    timer->start();
    Value* value = NULL;
    while(1) {
        Version* v = versionManager->getVersion(k, interval, OP_READ);
        if (v == NULL) {
            break;
        }
        if ((v->state() == COMMITTED) && ((lockInfo = getReadLock(v,interval)) !=  NULL)) {
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
    LockInfo* lockInfo;
    if ((prev->maxReadFrom >= interval.end) || ((lockInfo = getWriteLock(prev,interval)) == NULL)) {
        abortTransaction(tid);    
        return new ClientReply(tid, ABORT);
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
        ws_entry->version->maxReadFrom = ts;
        ws_entry->version->state = COMMITTED;
        lockManager->persist(ws_entry->version);
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

//TODO add code handling transactions aborted by the recovery manager
