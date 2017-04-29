#include "VersionManager.h"

std::shared_ptr<VersionManager::VersionEntry> VersionManager::CreateNewEntry(Key k) {
    std::shared_ptr<VersionEntry> entry(new VersionEntry(k));
    std::pair<std::unordered_map<Key,std::shared_ptr<VersionEntry>>::iterator,bool> ret 
        = all_versions.insert(std::pair<Key,std::shared_ptr<VersionEntry>>(k,entry));
    if (ret.second == false) {
        //delete entry;
        //entry = ret.first->second;
        entry = NULL;
    }

    std::shared_ptr<LockManager> lock(new LockManager(k));
    std::pair<std::unordered_map<Key,std::shared_ptr<LockManager>>::iterator,bool> suss 
        = locks_manager.insert(std::pair<Key,std::shared_ptr<LockManager>>(k,lock));
    if (suss.second == false){
        //delete lock;
        lock = NULL;
    }
    return entry;
}

std::shared_ptr<VersionManager::VersionEntry> VersionManager::GetVersionList(Key key){
    auto it = all_versions.find(key);
    if(it != all_versions.end()){
        return it->second;
    }
    return NULL;
}

void VersionManager::TryReadLock(TransactionId tid, Key key, TimestampInterval interval, LockInfo& lockInfo)
{
    #ifdef DEBUG
        std::cout<<"VersionManager: Read Key:"<<key<<",Timestamp interval["<<interval.start<<","<<interval.finish<<"]"<<endl;
    #endif

    std::shared_ptr<VersionEntry> ve = GetVersionList(key);
    if(ve == NULL){
        ve = CreateNewEntry(key);
        interval.lock_start = 0;
        locks_manager[key]->LockReadInterval(tid, interval);
        lockInfo.state = OperationState::FAIL_NO_VERSION;
        return;
    }

    Version* prev;
    Version* curr;

    /*Policy one */
    // curr = ve->versions.find(interval.start, prev);
    // TimestampInterval candidateInterval;
    // candidateInterval.start = prev->key;
    // //There are many policies here. Looking As much as possible.
    // candidateInterval.finish = min(interval.finish,curr->key);

    /*Policy two*/
    ve->lockEntry();
    curr = ve->versions.find(interval.finish, prev);
    TimestampInterval candidateInterval;
    candidateInterval.start = interval.start;
    candidateInterval.lock_start = prev->key;
    candidateInterval.finish = min(interval.finish,curr->key);
    bool is_success = locks_manager[key]->LockReadInterval(tid, candidateInterval);
    ve->unlockEntry();
    if(!is_success){
        lockInfo.state = OperationState::FAIL_PENDING_VERSION;
        return;
    }

    //lockInfo.locked.start = prev->key;
    //lockInfo.locked.finish = min(interval.finish,curr->key);
    lockInfo.version = prev;
    lockInfo.locked = candidateInterval;
    lockInfo.state = OperationState::R_LOCK_SUCCESS;
}


void VersionManager::TryWriteLock(TransactionId tid, Key key, TimestampInterval interval, LockInfo& lockInfo)
{

    #ifdef DEBUG
        std::cout<<"VersionManager: Write Key:"<<key<<",Timestamp interval ["<<interval.start<<","<<interval.finish<<"]"<<endl;
    #endif

    std::shared_ptr<VersionEntry> ve = GetVersionList(key);
    if (ve == NULL) {
        ve = CreateNewEntry(key);
    }

    Version* prev;
    Version* curr;
    /* Policy 1*/
    // curr = ve->versions.find(interval.start, prev);
    // TimestampInterval candidateInterval;
    // candidateInterval.start = max(prev->key,interval.start);
    // candidateInterval.finish = min(curr->key,interval.finish);

    /* Policy 2*/
    ve->lockEntry();
    curr = ve->versions.find(interval.finish, prev);
    TimestampInterval candidateInterval;
    candidateInterval.start = max(prev->key,interval.start);
    candidateInterval.finish = min(curr->key,interval.finish);
    
    //One Policy
    // if(interval.finish > curr->key)
    // {
    //  candidateInterval.start = curr->key;
    //  candidateInterval.finish =  interval.finish;
    // }
    // else
    // {
    //  candidateInterval.start = prev->key;
    //  candidateInterval.finish = min(interval.finish,curr->key);
    // }

    bool is_success = locks_manager[key]->LockWriteInterval(tid, candidateInterval);
    ve->unlockEntry();
    if(!is_success){
        lockInfo.state = OperationState::FAIL_PENDING_VERSION;
        return;
    }

    //lockInfo.locked.start = prev->key;
    //lockInfo.locked.finish = min(interval.finish,curr->key);
    lockInfo.version = prev;
    lockInfo.locked = candidateInterval;
    lockInfo.state = OperationState::W_LOCK_SUCCESS;
}

bool VersionManager::UpdateAndPersistVersion(TransactionId tid, Key key, Value value, Timestamp new_ts) {

    #ifdef DEBUG
        std::cout<<"VersionManager: Persist Key:"<<key<<",Timestamp:"<<new_ts<<endl;
    #endif

    std::shared_ptr<VersionEntry> ve = GetVersionList(key);
    ve->lockEntry();
    ve->versions.insert(new_ts,value);
    locks_manager[key]->CommitInterval(tid,new_ts);
    ve->unlockEntry();
    return true;
}

bool VersionManager::RemoveVersion(Key key, TimestampInterval interval){
    std::shared_ptr<VersionEntry> ve = GetVersionList(key);
    #ifdef DEBUG
        std::cout<<"VersionManager: Remove Key:"<<key<<",Timestamp interval["<<interval.start<<","<<interval.finish<<"]"<<endl;
    #endif
    ve->lockEntry();
    bool suss = locks_manager[key]->RemoveLock(interval);
    ve->unlockEntry();
    return suss;
}

bool VersionManager::GarbageCollection(Timestamp time){
    #ifdef DEBUG
        std::cout<<"VersionManager: Garbage Collection:"<<time<<endl;
    #endif
    bool suss = true;
    for(auto v:all_versions){
        v.second->lockEntry();
        v.second->versions.erase(time);
        suss &= locks_manager[v.first]->GarbageCollection(time);
        v.second->unlockEntry();
    }
    return suss;
}