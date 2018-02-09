#include "VersionManager.h"

void VersionManager::TryReadLock(TransactionId tid, Key key, TimestampInterval interval, LockInfo& lockInfo)
{
	#ifdef DEBUG
		//std::cout<<"VersionManager: Read Key:"<<key<<",Timestamp interval["<<interval.start<<","<<interval.finish<<"]"<<endl;
	#endif

	std::shared_ptr<VersionEntry> ve;
	if(all_versions.find(key, ve)){
		Version* prev;
		Version* curr;
		/*Policy one */
		ve->lockEntry();
		if(s_policy == LOCK_FIRST_INTERVAL)
		{
			curr = ve->versions.find(interval.start, prev);
		}
		/*Policy two*/
		else if(s_policy == LOCK_LAST_INTERVAL)
		{
			curr = ve->versions.find(interval.finish, prev);
		}

		TimestampInterval candidateInterval;
		candidateInterval.start = interval.start;
		candidateInterval.lock_start = prev->key + 1;
		candidateInterval.finish = min(interval.finish, curr->key - 1);
		/*
		if(candidateInterval.lock_start > candidateInterval.finish){
			lockInfo.state = OperationState::FAIL_PENDING_VERSION;
			return;
		}
		*/
		bool is_success = ve->manager.LockReadInterval(tid, candidateInterval);
		ve->unlockEntry();
		if(!is_success){
			lockInfo.state = OperationState::FAIL_PENDING_VERSION;
			return;
		}
		lockInfo.version = prev;
		lockInfo.locked = candidateInterval;
		lockInfo.state = OperationState::R_LOCK_SUCCESS;
	}
	else
	{
		//std::shared_ptr<VersionEntry> ve(new VersionEntry(key));
		ve = std::make_shared<VersionEntry>(key);
		interval.lock_start = 1;
		ve->manager.LockReadInterval(tid, interval);
		all_versions.insert(key, ve);
		lockInfo.locked = interval;
		lockInfo.state = OperationState::FAIL_NO_VERSION;
	}
}


void VersionManager::TryWriteLock(TransactionId tid, Key key, TimestampInterval interval, LockInfo& lockInfo)
{

	#ifdef DEBUG
		//std::cout<<"VersionManager: Write Key:"<<key<<",Timestamp interval ["<<interval.start<<","<<interval.finish<<"]"<<endl;
	#endif

	std::shared_ptr<VersionEntry> ve;
	if(!all_versions.find(key, ve)){
		ve = std::make_shared<VersionEntry>(key);
		all_versions.insert(key, ve);
	}

	TimestampInterval candidateInterval;
	candidateInterval.start = interval.start;
	candidateInterval.finish = interval.finish;

	ve->lockEntry();
	bool is_success = ve->manager.LockWriteInterval(tid, candidateInterval);
	ve->unlockEntry();

	if(!is_success){
		lockInfo.state = OperationState::FAIL_PENDING_VERSION;
		return;
	}
	lockInfo.locked = candidateInterval;
	lockInfo.state = OperationState::W_LOCK_SUCCESS;
}

bool VersionManager::UpdateAndPersistVersion(TransactionId tid, Key key, Value value, Timestamp old_ts, Timestamp new_ts) {

	#ifdef DEBUG
		//std::cout<<"VersionManager: Persist Key:"<<key<<",Timestamp:"<<new_ts<<endl;
	#endif

	std::shared_ptr<VersionEntry> ve;
	if(all_versions.find(key, ve)){
		ve->lockEntry();
		ve->versions.insert(new_ts,value);
		ve->manager.CommitInterval(tid, old_ts, new_ts);
		ve->unlockEntry();
	}
	return true;
}

bool VersionManager::PersistReadLock(TransactionId tid, Key key, Timestamp old_ts, Timestamp new_ts){
 	std::shared_ptr<VersionEntry> ve;
 	if(all_versions.find(key, ve)){
 		ve->lockEntry();
 		ve->manager.CommitInterval(tid, old_ts, new_ts);
 		ve->unlockEntry();
 	}
 	return true;
}

bool VersionManager::RemoveVersion(TransactionId tid, Key key, Timestamp old_ts){
	
	#ifdef DEBUG
		//std::cout<<"VersionManager: Remove Key:"<<key<<endl;
	#endif
	bool suss = true;
	std::shared_ptr<VersionEntry> ve;
	if(all_versions.find(key, ve)){
		ve->lockEntry();
		suss &= ve->manager.RemoveLock(tid, old_ts);
		ve->unlockEntry();
	}
	return suss;
}

bool VersionManager::GarbageCollection(Timestamp ts){
	#ifdef DEBUG
		std::cout<<"VersionManager: Garbage Collection:"<<ts<<std::endl;
	#endif
	bool suss = true;
	{
		auto vl = all_versions.lock_table();
		for (const auto &ve : vl) {
			ve.second->lockEntry();
			ve.second->versions.erase(ts);
			suss &= ve.second->manager.GarbageCollection(ts);
			ve.second->unlockEntry();
		}
	}
	return suss;
}

bool VersionManager::GetSize(int& version_len, int& lock_len){
	bool suss = true;
	{
		auto vl = all_versions.lock_table();
		for (const auto &ve : vl) {
			version_len += ve.second->versions.size();
			lock_len += ve.second->manager.GetSize();
		}
	}
	return suss;
}
