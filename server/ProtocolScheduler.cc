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


    std::map<TransactionId, std::queue<WSEntry*>*>::iterator it = pendingWriteSets.find(tid); //TODO: do struct for this, containing a version, a value, and a lock
    it->second->push(wse);
    return new ClientReply(tid, WRITE_REPLY, lockInfo);
}

ClientReply* ProtocolScheduler::handleCommit(TransactionId tid, Timestamp ts) {
#ifdef DEBUG
    std::cout<<"Handling commit: Transaction id "<<tid<<"; Timestamp "<<ts<<" ."<<endl;
#endif
    std::queue<WSEntry*>* ws = pendingWriteSets.find(tid)->second; 
    if (ws == NULL) {
        return new ClientReply(tid, (ReplyType)WRITES_NOT_FOUND);
    }
    while (!ws->empty()) {
        //TODO do I need to protect this with locks?
        WSEntry * ws_entry = ws->front();
        ws->pop();
        versionManager->updateAndPersistVersion(ws_entry->key, ws_entry->version, ts, 0, ts, COMMITTED);
        //ws_entry->version->timestamp = ts; 
        //if (ws_entry->version->maxReadFrom < ts) {
            //ws_entry->version->maxReadFrom = ts;
        //}
        //ws_entry->version->state = COMMITTED;
        //versionManager->persistVersion(ws_entry->key, ws_entry->version);
#ifndef INITIAL_TESTING
        dataStore.write(toDsKey(ws_entry->key,ts), ws_entry->value);
#endif
        delete(ws_entry);
        //TODO: delete members as well?
    }
    pendingWriteSets.erase(tid);
    //removePendingWriteSet(tid);
    return new ClientReply(tid, (ReplyType)COMMIT_ACK);
}

ClientReply* ProtocolScheduler::handleAbort(TransactionId tid) {
#ifdef DEBUG
    std::cout<<"Handling abort: Transaction id "<<tid<<" ."<<endl;
#endif
    std::queue<WSEntry*>* ws = pendingWriteSets.find(tid)->second;
    if (ws == NULL) {
        return new ClientReply(tid, (ReplyType)WRITES_NOT_FOUND);
    }
    while (!ws->empty()) {
        //TODO do I need to protect this with locks?
        WSEntry * ws_entry = ws->front();
        ws->pop();
        versionManager->removeVersion(ws_entry->key, ws_entry->version);
        delete(ws_entry);
        //TODO: what else needs to be deleted? is the version deleted by removeVersion()?
    }

    pendingWriteSets.erase(tid);
    return new ClientReply(tid,(ReplyType)ABORT_ACK);
}

ClientReply* ProtocolScheduler::handleHintRequest(TimestampInterval interval, Key k) {
#ifdef DEBUG
    std::cout<<"Handling hint request: Timestamp interval ["<<interval.start<<","<<interval.end<<"]; Key "<<k<<" ."<<endl;
#endif
    return NULL;
}

ClientReply* ProtocolScheduler::handleSingleKeyOperation(TransactionId tid, std::string opName, TimestampInterval interval, Key k, Value v) {
#ifdef DEBUG
    std::cout<<"Handling single key operation: Transaction id "<<tid<<"; Operation "<<opName<<"; Timestamp interval ["<<interval.start<<","<<interval.end<<"]; Key "<<k<<"; Value "<<v<<" ."<<endl;
#endif

    return NULL;
}

DsKey* ProtocolScheduler::toDsKey(Key k, Timestamp ts) {
    return new DsKey("not implemented");
}

//TODO add code handling transactions aborted by the recovery manager
