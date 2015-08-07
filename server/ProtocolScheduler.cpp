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

#include "ProtocolScheduler.h"

using namespace std;
using namespace TxProtocol;

ProtocolScheduler::ProtocolScheduler() {

}

ProtocolScheduler::~ProtocolScheduler() {
}

void ProtocolScheduler::handleReadRequest(ReadReply& _return, const TransactionId tid, const TimestampInterval& interval, const Key& k, const bool isReadOnly) {
    Timer timer;
    int numRetries = 0;
#ifdef DEBUG
    std::cout<<"Handling read: Transaction id "<<tid<<"; Timestamp interval ["<<interval.start<<","<<interval.end<<"]; Key "<<k<<" ."<<endl;
#endif
    LockInfo lockInfo; 
    timer.start();
    Value value;
    while(1) {
        versionManager.tryReadLock(k,interval,lockInfo);
        if (lockInfo.state == OperationState::R_LOCK_SUCCESS) {
#ifndef INITIAL_TESTING
            value = dataStore->read(toDsKey(k,lockInfo.potential.start)); //assuming if state is R_LOCK_SUCCESS potential.start is always lockInfo->version->timestamp
#else
            value = "not implemented";
#endif
            break;
        }
        if (lockInfo.state == OperationState::FAIL_NO_VERSION) {
            _return.tid = tid;
            _return.state = lockInfo.state;
            _return.key = k;
            _return.value = "";
            _return.interval.start = 0;
            _return.interval.finish = 0;
            _return.potential.start = 0;
            _return.potential.finish = 0;
            return;
        }
 
        if (!isReadOnly && (numRetries > MIN_NUM_RETRIES) && (timer.timeout())) {
            _return.tid = tid;
            _return.state = lockInfo.state; //should be FAIL_PENDING_VERSION
            _return.key = k;
            _return.value = "";
            _return.interval.start = 0;
            _return.interval.finish = 0;
            _return.potential.start = 0;
            _return.potential.finish = 0;
            return;
        }
#ifndef INITIAL_TESTING
       pause(PAUSE_LENGTH);
#endif
        numRetries++;
    }
    _return.tid = tid;
    _return.state = lockInfo.state;
    _return.key = k;
    _return.value = value;
    _return.interval = lockInfo.locked;
    _return.potential = lockInfo.potential;
    return;
}


void ProtocolScheduler::handleWriteRequest(WriteReply& _return, const TransactionId tid, const TimestampInterval& interval, const Key& k, const Value& v) {
#ifdef DEBUG
    std::cout<<"Handling write: Transaction id "<<tid<<"; Timestamp interval ["<<interval.start<<","<<interval.end<<"]; Key "<<k<<"; Value "<<v<<" ."<<endl;
#endif
    LockInfo lockInfo;
    versionManager.tryWriteLock(k, interval, lockInfo);

    if (lockInfo.state != OperationState::W_LOCK_SUCCESS) {
        abortTransaction(tid);
        _return.tid = tid;
        _return.state = lockInfo.state;
        _return.interval = lockInfo.locked;
        _return.potential = lockInfo.potential;
        _return.key = k;
        return;
    }
    
    WriteSet* ws;
    if (!pendingWriteSets.find(tid, ws)) {
        ws = new WriteSet();
        pendingWriteSets.insert(tid, ws);//TODO also stamp the write set with a timestamp; if too much time has passed and no commit received, may have to act
    }

//    if (pendingWriteSets.find(tid) == pendingWriteSets.end()){
        //auto q = new std::unordered_set<<shared_ptr<WSEntry>>;
        //pendingWriteSets.insert(std::pair<TransactionId,std::unordered_set<shared_ptr<WSEntry>>*>(tid, q)); 
    //}

    //auto wse = make_shared<WriteSetEntry>(lockInfo.version,k,v);
    //auto it = pendingWriteSets.find(tid); 
    //it->second->insert(wse);
    ws->lock();
    ws->insert(new WriteSetEntry(lockInfo.version, k, v));
    ws->unlock();
    _return.tid = tid;
    _return.state = lockInfo.state;
    _return.interval = lockInfo.locked;
    _return.potential = lockInfo.potential;
    _return.key = k;
    return;
}


void ProtocolScheduler::handleHintRequest(HintReplyl& _return, const TransactionId tid, const TimestampInterval& interval, const Key& k) {
    LockInfo lockInfo;
#ifdef DEBUG
    std::cout<<"Handling hint request: Timestamp interval ["<<interval.start<<","<<interval.end<<"]; Key "<<k<<" ."<<endl;
#endif
    versionManager.getWriteLockHint(k, interval, lockInfo);
    _return.interval = lockInfo.locked;
    _retrun.potential = lockInfo.potential;
    _return.key = k;
    _return.state = lockInfo.state;
    return;
}


void ProtocolScheduler::handleCommit(CommitReply& _return, const TransactionId tid, const Timestamp ts) {
#ifdef DEBUG
    std::cout<<"Handling commit: Transaction id "<<tid<<"; Timestamp "<<ts<<" ."<<endl;
#endif
    WriteSet* ws;
    //= pendingWriteSets.find(tid)->second; 
    if (!pendingWriteSets.find(tid, ws)) {
        _return.state = OperationState::WRITES_NOT_FOUND;
        _return.tid = tid;
        return;
    }
    ws->lock();
    for (auto& ws_entry: ws->pendingWrites) {
        //TODO do I need to protect this with locks?
        versionManager.updateAndPersistVersion(ws_entry->key, ws_entry->version, ts, 0, ts, COMMITTED);
#ifndef INITIAL_TESTING
        dataStore.write(toDsKey(ws_entry->key,ts), ws_entry->value);
#endif
    }
    ws->clear();
    ws->unlock();
    pendingWriteSets.erase(tid);
    _return.state = OperationState::COMMIT_OK;
    _return.tid = tid;
    return;
}

void ProtocolScheduler::handleAbort(AbortReply& _return, const TransactionId tid) {
#ifdef DEBUG
    std::cout<<"Handling abort: Transaction id "<<tid<<" ."<<endl;
#endif
    WriteSet* ws;
    //auto ws = pendingWriteSets.find(tid)->second;
    if (!pendingWriteSets.find(tid, ws)) {
        _return.state = OperationState::WRITES_NOT_FOUND;
        _return.tid = tid;
        return;
    }
    
    ws->lock;
    for (auto& ws_entry: ws->pendingWrites) {
        //TODO do I need to protect this with locks?
        versionManager.removeVersion(ws_entry->key, ws_entry->version);
        //TODO: what else needs to be deleted? is the version deleted by removeVersion()?
    }
    ws->clear();
    ws->unlock();

    pendingWriteSets.erase(tid);
    _return.state = OperationState::ABORT_OK;
    _return.tid = tid;
    return;
}


void ProtocolScheduler::handleOperation(ServerGenericReply& _return, const ClientGenericRequest& cr) {
#ifdef DEBUG
    std::cout<<"Handling single key operation: Transaction id "<<tid<<"; Operation "<<opName<<"; Timestamp interval ["<<interval.start<<","<<interval.end<<"]; Key "<<k<<"; Value "<<v<<" ."<<endl;
#endif
    _return.state = OperationState::NOT_IMPLEMENTED;
    return;
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


void ProtocolScheduler::handleExpandRead(ExpandReadReply& _return, const TransactionId tid, const Timestamp versionTimestamp, const TimestampInterval& newInterval, const Key& k) {
    LockInfo lockInfo;
    versionManager.tryExpandRead(k, versionTimestamp, newInterval, lockInfo); 
    _return.state = lockInfo.state;
    _return.interval = lockInfo.locked;
    _return.potential = lockInfo.potential;
    _return.key = k;
    _return.tid = tid;
    return; 
}

void ProtocolScheduler::handleExpandWrite(ExpandWriteReply& _return, const TransactionId tid, const Timestamp versionTimestamp, const TimestampInterval& newInterval, const Key& k) {
    Value value;
    LockInfo lockInfo;

    auto ws = pendingWriteSets.find(tid)->second;
    if (ws == NULL) {
        _return.state = OperationState::WRITES_NOT_FOUND;
        _return.tid = tid;
        return;
    }

    bool found = false;

    for (auto& ws_entry: ws) {
        if (ws_entry->key == key) {
            found = true;
            value = ws_entry->value;
            versionManager.tryExpandWrite(ws_entry->key, versionTimestamp, newInterval, lockInfo);
            ws->erase(ws_entry);
            break;
        }
    }

    if (found == false) {
        _return.state = OperationState::WRITES_NOT_FOUND;
        _return.tid = tid;
        return;
    }

    if (lockInfo.state != OperationState::W_LOCK_SUCCESS) {
        abortTransaction(tid);
        _return.tid = tid;
        _return.state = lockInfo.state;
        _return.interval = lockInfo.locked;
        _return.potential = lockInfo.potential;
        _return.key = k;
        return;
    }
    
    auto wse = make_shared<WriteSetEntry>(lockInfo.version,k,value);
    ws->insert(wse);

    _return.tid = tid;
    _return.state = EXPANSION_OK;
    _return.interval = lockInfo.locked;
    _return.potential = lockInfo.potential;
    _return.key = k;
    return;
}

//TODO add code handling transactions aborted by the recovery manager
