#include "VersionManager.h"

std::shared_ptr<VersionManager::VersionEntry> VersionManager::CreateNewEntry(Key key) {

	std::shared_ptr<VersionManager::VersionEntry> entry(new VersionEntry(key));
	lockSet.lock(key);
	std::pair<std::unordered_map<Key,std::shared_ptr<VersionEntry>>::iterator,bool> ret_version
		= all_versions.insert(std::pair<Key,std::shared_ptr<VersionEntry>>(key,entry));
	if (ret_version.second == false) {
		//delete entry;
		entry = ret_version.first->second;
	}
	lockSet.unlock(key);

	return entry;
}

std::shared_ptr<VersionManager::VersionEntry> VersionManager::GetVersionList(Key key){

	lockSet.lock(key);
	std::shared_ptr<VersionManager::VersionEntry> ret;
	auto it = all_versions.find(key);
	if(it != all_versions.end()){
		ret = it->second;
	}
	else{
		ret = NULL;
	}
	lockSet.unlock(key);
	return ret;
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
		ve->lockEntry();
		ve->manager.LockReadInterval(tid, interval);
		ve->unlockEntry();
		lockInfo.state = OperationState::FAIL_NO_VERSION;
		return;
	}

	Version* prev;
	Version* curr;
	ve->lockEntry();
	
	/*Policy one */
	if(s_policy == LOCK_FIRST_INTERVAL)
	{
		curr = ve->versions.find(interval.start, prev);
		// TimestampInterval candidateInterval;
		// candidateInterval.start = prev->key;
		// //There are many policies here. Looking As much as possible.
		// candidateInterval.finish = min(interval.finish,curr->key);
	}
	/*Policy two*/
	else if(s_policy == LOCK_LAST_INTERVAL)
	{
		curr = ve->versions.find(interval.finish, prev);
	}
	TimestampInterval candidateInterval;
	candidateInterval.start = interval.start;
	candidateInterval.lock_start = prev->key + 1;
	candidateInterval.finish = min(interval.finish,curr->key - 1);
	bool is_success = ve->manager.LockReadInterval(tid, candidateInterval);
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

	//Version* prev;
	//Version* curr;
	/* Policy 1*/
	// curr = ve->versions.find(interval.start, prev);
	// TimestampInterval candidateInterval;
	// candidateInterval.start = max(prev->key,interval.start);
	// candidateInterval.finish = min(curr->key,interval.finish);

	/* Policy 2*/
	ve->lockEntry();
	//curr = ve->versions.find(interval.finish, prev);
	TimestampInterval candidateInterval;
	candidateInterval.start = interval.start;
	//max(prev->key + 1,interval.start);
	candidateInterval.finish = interval.finish;
	//min(curr->key - 1,interval.finish);
	
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

	bool is_success = ve->manager.LockWriteInterval(tid, candidateInterval);
	ve->unlockEntry();
	if(!is_success){
		lockInfo.state = OperationState::FAIL_PENDING_VERSION;
		return;
	}

	// lockInfo.locked.start = prev->key;
	// lockInfo.locked.finish = min(interval.finish,curr->key);
	//lockInfo.version = prev;
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
	ve->manager.CommitInterval(tid,new_ts);
	ve->unlockEntry();
	//GC
	//GarbageCollection(new_ts - (100 << 8));
	return true;
}

bool VersionManager::RemoveVersion(TransactionId tid, Key key, TimestampInterval interval){
	std::shared_ptr<VersionEntry> ve = GetVersionList(key);
	#ifdef DEBUG
		std::cout<<"VersionManager: Remove Key:"<<key<<",Timestamp interval["<<interval.start<<","<<interval.finish<<"]"<<endl;
	#endif
	ve->lockEntry();
	bool suss = ve->manager.RemoveLock(tid, interval);
	ve->unlockEntry();
	return suss;
}

bool VersionManager::GarbageCollection(Timestamp time){
	#ifdef DEBUG
		std::cout<<"VersionManager: Garbage Collection:"<<time<<endl;
	#endif
	bool suss = true;
	for(auto const &v:all_versions){
		v.second->lockEntry();
		v.second->versions.erase(time);
		suss &= v.second->manager.GarbageCollection(time);
		v.second->unlockEntry();
	}
	return suss;
}
