#include "VersionManager.h"

VersionManager::VersionEntry* VersionManager::CreateNewEntry(Key k) {
    VersionEntry* entry = new VersionEntry(k);
    std::pair<std::unordered_map<Key,VersionEntry*>::iterator,bool> ret 
        = committed_version.insert(std::pair<Key,VersionEntry*>(k,entry));
    if (ret.second == false) {
        delete entry;
        entry = ret.first->second;
    }

    LockManager* lock = new LockManager(k);
    std::pair<std::unordered_map<Key,LockManager*>::iterator,bool> suss 
        = locks_manager.insert(std::pair<Key,LockManager*>(k,lock));
    if (suss.second == false)
        delete lock;
    return entry;
}

VersionManager::VersionEntry* VersionManager::GetVersionList(Key key){
	if(committed_version.count(key) > 0){
		return committed_version[key];
	}
	return NULL;
}

void VersionManager::TryReadLock(TransactionId tid, Key key, TimestampInterval interval, LockInfo& lockInfo)
{
    #ifdef DEBUG
        std::cout<<"VersionManager: Read Key:"<<key<<",Timestamp interval["<<interval.start<<","<<interval.finish<<"]"<<endl;
    #endif

	VersionEntry* ve = GetVersionList(key);
	if(ve == NULL){
        ve = CreateNewEntry(key);
        interval.start = 0;
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
    curr = ve->versions.find(interval.finish, prev);
    TimestampInterval candidateInterval;
    candidateInterval.start = prev->key;
    candidateInterval.finish = min(interval.finish,curr->key);

    bool is_success = locks_manager[key]->LockReadInterval(tid, candidateInterval);
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

	VersionEntry* ve = GetVersionList(key);
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
    curr = ve->versions.find(interval.finish, prev);
    TimestampInterval candidateInterval;
    candidateInterval.start = max(prev->key,interval.start);
    candidateInterval.finish = min(curr->key,interval.finish);
    
    //One Policy
    // if(interval.finish > curr->key)
    // {
    // 	candidateInterval.start = curr->key;
    // 	candidateInterval.finish =  interval.finish;
    // }
    // else
    // {
    // 	candidateInterval.start = prev->key;
    // 	candidateInterval.finish = min(interval.finish,curr->key);
    // }

    bool is_success = locks_manager[key]->LockWriteInterval(tid, candidateInterval);
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

    VersionEntry* ve = GetVersionList(key);
    ve->versions.insert(new_ts,value);
    locks_manager[key]->CommitInterval(tid,new_ts);
    return true;
}

bool VersionManager::RemoveVersion(Key key, TimestampInterval interval){
    //VersionEntry* ve = GetVersionList(key);
    #ifdef DEBUG
        std::cout<<"VersionManager: Remove Key:"<<key<<",Timestamp interval["<<interval.start<<","<<interval.finish<<"]"<<endl;
    #endif
    bool suss = locks_manager[key]->RemoveLock(interval);
    return suss;
}