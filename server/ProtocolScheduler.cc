#include "ProtocolScheduler.h"

using namespace std;

ProtocolScheduler::ProtocolScheduler() {

}

ProtocolScheduler::~ProtocolScheduler() {
}

void ProtocolScheduler::handleReadRequest(ReadReply& _return, const TransactionId tid, const TimestampInterval& ts, const Key& k) {
    Timer timer;
#ifdef DEBUG
    std::cout<<"Handling read: Transaction id "<<tid<<"; Timestamp interval ["<<interval.start<<","<<interval.end<<"]; Key "<<k<<" ."<<endl;
#endif
    LockInfo lockInfo; 
    timer.start();
    Value* value = NULL;
    while(1) {
        versionManager.tryReadLock(k,interval,lockInfo);
        if (lockInfo.state == R_LOCK_SUCCESS) {
#ifndef INITIAL_TESTING
            value = dataStore->read(toDsKey(k,lockInfo.version->timestamp));
#else
            std::string *str = new std::string("not implemented");
            value = str;
#endif
            break;
        }
        if (lockInfo.state == FAIL_NO_VERSION) {
            _return.tid = tid;
            _return.state = lockInfo.state;
            _return.key = k;
            _return.value = NULL;
            _return.interval = {0,0};
            _return.potential = {0,0};
            return;
        }
 
        if (timer.timeout()) {
            _return.tid = tid;
            _return.state = lockInfo.state;
            _return.key = k;
            _return.value = NULL;
            _return.interval = {0,0};
            _return.potential = {0,0};
            return;
        }
#ifndef INITIAL_TESTING
       pause(PAUSE_LENGTH);
#endif
    }
    _return.tid = tid;
    _return.state = lockInfo.state;
    _return.key = k;
    _return.value = value;
    _return.interval = lockInfo.value;
    _return.potential = lockInfo.potential;
    return;
}


void ProtocolScheduler::handleWriteRequest(WriteReply& _return, const TransactionId tid, const TimestampInterval& ts, const Key& k, const Value& v) {
#ifdef DEBUG
    std::cout<<"Handling write: Transaction id "<<tid<<"; Timestamp interval ["<<interval.start<<","<<interval.end<<"]; Key "<<k<<"; Value "<<v<<" ."<<endl;
#endif
    LockInfo lockInfo;
    versionManager.tryWriteLock(k, interval, lockInfo);

    if (lockInfo.state != W_LOCK_SUCCESS) {
        abortTransaction(tid);
        _return.tid = tid;
        _return.state = lockInfo.state;
        _return.interval = lockInfo.interval;
        _return.potential = lockInfo.potential;
        _return.key = k;
        return;
    }
    
    if (pendingWriteSets.find(tid) == pendingWriteSets.end()){
        std::queue<WSEntry*>* q = new std::queue<WSEntry*>;
        pendingWriteSets.insert(std::pair<TransactionId,std::queue<WSEntry*>*>(tid, q)); //TODO also stamp the write set with a timestamp; if too much time has passed and no commit received, may have to act
    }
    WSEntry* wse = new WSEntry(lockInfo.version,k,v);
    std::map<TransactionId, std::queue<WSEntry*>*>::iterator it = pendingWriteSets.find(tid); 
    it->second->push(wse);
    _return.tid = tid;
    _return.state = lockInfo.state;
    _return.interval = lockInfo.interval;
    _return.potential = lockInfo.potential;
    _return.key = k;
    return;
}


void ProtocolScheduler::handleHintRequest(TimestampInterval& _return, const TransactionId tid, const TimestampInterval& ts, const Key& k) {
#ifdef DEBUG
    std::cout<<"Handling hint request: Timestamp interval ["<<interval.start<<","<<interval.end<<"]; Key "<<k<<" ."<<endl;
#endif
    TimestampInterval res = versionManager.getWriteLockHint(k, interval);
    _return = res;
    return;
}


void handleCommit(CommitReply& _return, const TransactionId tid, const Timestamp ts) {
#ifdef DEBUG
    std::cout<<"Handling commit: Transaction id "<<tid<<"; Timestamp "<<ts<<" ."<<endl;
#endif
    std::queue<WSEntry*>* ws = pendingWriteSets.find(tid)->second; 
    if (ws == NULL) {
        _return.state = WRITES_NOT_FOUND;
        _return.tid = tid;
        return;
    }
    while (!ws->empty()) {
        //TODO do I need to protect this with locks?
        WSEntry * ws_entry = ws->front();
        ws->pop();
        versionManager.updateAndPersistVersion(ws_entry->key, ws_entry->version, ts, 0, ts, COMMITTED);
#ifndef INITIAL_TESTING
        dataStore.write(toDsKey(ws_entry->key,ts), ws_entry->value);
#endif
        delete(ws_entry);
        //TODO: delete members as well?
    }
    pendingWriteSets.erase(tid);
    _return.state = COMMIT_OK;
    _return.tid = tid;
    return;
}

void handleAbort(AbortReply& _return, const TransactionId tid) {
#ifdef DEBUG
    std::cout<<"Handling abort: Transaction id "<<tid<<" ."<<endl;
#endif
    std::queue<WSEntry*>* ws = pendingWriteSets.find(tid)->second;
    if (ws == NULL) {
        _return.state = WRITES_NOT_FOUND;
        _return.tid = tid;
        return;
    }
    while (!ws->empty()) {
        //TODO do I need to protect this with locks?
        WSEntry * ws_entry = ws->front();
        ws->pop();
        versionManager.removeVersion(ws_entry->key, ws_entry->version);
        delete(ws_entry);
        //TODO: what else needs to be deleted? is the version deleted by removeVersion()?
    }

    pendingWriteSets.erase(tid);
    _return.state = ABORT_OK;
    _return.tid = tid;
    return;
}


void ProtocolScheduler::handleOperation(ServerGenericReply& _return, const ClientGenericRequest& cr) {
//ClientReply* ProtocolScheduler::handleSingleKeyOperation(TransactionId tid, std::string opName, TimestampInterval interval, Key k, Value v) {
#ifdef DEBUG
    std::cout<<"Handling single key operation: Transaction id "<<tid<<"; Operation "<<opName<<"; Timestamp interval ["<<interval.start<<","<<interval.end<<"]; Key "<<k<<"; Value "<<v<<" ."<<endl;
#endif

    return NULL;
}

DsKey* ProtocolScheduler::toDsKey(Key k, Timestamp ts) {
    return new DsKey("not implemented");
}

void ProtocolScheduler::abortTransaction(TransactionId tid) {
}

#ifndef INITIAL_TESTING
//sent by the timestamp oracle
void ProtocolScheduler::handleNewEpoch(Timestamp barrier) {
    //garbage collection
    std::queue<Timestamp> q;
    //iterate over the keys in the version manager
    for ( std::map<Key, VersionManagerEntry*>::iterator it = versionManager.versions.begin(); it != versionManager.versions.end; ++it) {
        VersionManagerEntry* ve = it->second;
        //for each key obtain a list of timestamps that are deleted (in the process delete them form the version manager)
        ve->collectTo(barrier, &q);
        while (!q.empty()){
            //for each timestamp in the list remove key:timestamp 
            Timestamp t = q.front();
            versionManager.persistRemoveVersion(ve->key,t);
            dataStore.remove(toDsKey(ve->key,t));
            q.pop();
        }
    }
    //TODO don't accept messages from clients older than barrier anymore
}
#endif

//TODO add code handling transactions aborted by the recovery manager
