#include "ProtocolScheduler.h"

ProtocolScheduler::ProtocolScheduler() {

}

ProtocolScheduler::~ProtocolScheduler() {
}

ClientReply* ProtocolScheduler::handleRead(TransactionId tid, TimestampInterval interval, Key k) {
#ifdef DEBUG
    std::cout<<"Handling read: Transaction id "<<tid<<"; Timestamp interval ["<<interval.start<<","<<interval.end<<"]; Key "<<k<<" ."<<endl;
#endif
    LockInfo* lockInfo = new LockInfo(); //TODO need a better solution; shouldn't have to allocate a new object for every request
#ifndef INITIAL_TESTING
    timer->start();
#endif
    Value* value = NULL;
    while(1) {
        versionManager->tryReadLock(k,interval,lockInfo);
        if (lockInfo->state == R_LOCK_SUCCESS) {
#ifndef INITIAL_TESTING
            value = dataStore->read(toDsKey(k,lockInfo->version->timestamp));
#else
            std::string *str = new std::string("not implemented");
            value = str;
#endif
            break;
        }
#ifndef INITIAL_TESTING
        if (timer->timeout()) {
            return new ClientReply(tid, TIMEOUT);
        }
        pause(PAUSE_LENGTH);
#endif
    }
    return new ClientReply(tid, READ_REPLY, lockInfo, value);
}

ClientReply* ProtocolScheduler::handleWrite(TransactionId tid, TimestampInterval interval, Key k, Value v) {
#ifdef DEBUG
    std::cout<<"Handling write: Transaction id "<<tid<<"; Timestamp interval ["<<interval.start<<","<<interval.end<<"]; Key "<<k<<"; Value "<<v<<" ."<<endl;
#endif
    //Version* prev = getVersion(k,interval,OP_WRITE); 
    LockInfo* lockInfo = new LockInfo();
    versionManager->tryWriteLock(k, interval, lockInfo);

    if (lockInfo->state != W_LOCK_SUCCESS) {
        abortTransaction(tid);
        return new ClientReply(tid, WRITE_REPLY, lockInfo);
    }
    
    if (pendingWriteSets.find(tid) == pendingWriteSets.end()){
        std::queue<WSEntry*>* q = new std::queue<WSEntry*>;
        pendingWriteSets.insert(std::pair<TransactionId,std::queue<WSEntry*>*>(tid, q));
    }
    WSEntry* wse = new WSEntry(lockInfo->version,k,v);


    pendingWriteSets[tid].insert(vse); //TODO: do struct for this, containing a version, a value, and a lock
    return new ClientReply(tid, WRITE_REPLY, lockInfo);
}

ClientReply* ProtocolScheduler::handleCommit(TransactionId tid, Timestamp ts) {
#ifdef DEBUG
    std::cout<<"Handling commit: Transaction id "<<tid<<"; Timestamp "<<ts<<" ."<<endl;
#endif
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
#ifndef INITIAL_TESTING
        dataStore.write(toDsKey(ws_entry->key,ts), ws_entry->value);
#endif
        delete(ws_entry);
        //TODO: delete members as well?
    }
    removePendingWriteSet(tid);
    return new ClientReply(tid, COMMIT_ACK);
}

ClientReply* ProtocolScheduler::handleAbort(TransactionId tid) {
#ifdef DEBUG
    std::cout<<"Handling abort: Transaction id "<<tid<<" ."<<endl;
#endif
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
#ifdef DEBUG
    std::cout<<"Handling hint request: Timestamp interval ["<<interval.start<<","<<interval.end<<"]; Key "<<k<<" ."<<endl;
#endif
    return NULL;
}

ClientReply* ProtocolScheduler::handleSingleKeyOperation(std::string opName, TimestampInterval interval, Key k, Value v) {
#ifdef DEBUG
    std::cout<<"Handling single key operation: Transaction id "<<tid<<"; Operation "<<opName<<"; Timestamp interval ["<<interval.start<<","<<interval.end<<"]; Key "<<k<<"; Value "<<v<<" ."<<endl;
#endif

    return NULL;
}

std::string ProtocolScheduler::toDsKey(Key k, Timestamp ts) {
    return "not implemented";
}

//TODO add code handling transactions aborted by the recovery manager
