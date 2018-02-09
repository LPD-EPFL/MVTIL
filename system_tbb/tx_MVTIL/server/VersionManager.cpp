#include "VersionManager.h"

void VersionManager::TryReadLock(TransactionId tid, Key key, TimestampInterval interval, LockInfo& lockInfo)
{
	#ifdef DEBUG
		//std::cout<<"VersionManager: Read Key:"<<key<<",Timestamp interval["<<interval.start<<","<<interval.finish<<"]"<<endl;
	#endif

	VersionMap::accessor veAcc;
	if(all_versions.find(veAcc, key)){
		std::shared_ptr<VersionEntry> ve = veAcc->second;

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
		std::shared_ptr<VersionEntry> ve(new VersionEntry(key));
		interval.lock_start = 1;
		ve->manager.LockReadInterval(tid, interval);
		all_versions.insert(veAcc, key);
		veAcc->second = ve;
		lockInfo.locked = interval;
		lockInfo.state = OperationState::FAIL_NO_VERSION;
	}
	veAcc.release();
}


void VersionManager::TryWriteLock(TransactionId tid, Key key, TimestampInterval interval, LockInfo& lockInfo)
{

	#ifdef DEBUG
		//std::cout<<"VersionManager: Write Key:"<<key<<",Timestamp interval ["<<interval.start<<","<<interval.finish<<"]"<<endl;
	#endif

	VersionMap::accessor veAcc;
	if(!all_versions.find(veAcc, key)){
		std::shared_ptr<VersionEntry> newVe(new VersionEntry(key));
		all_versions.insert(veAcc, key);
		veAcc->second = newVe;
	}

	std::shared_ptr<VersionEntry> ve = veAcc->second;
	TimestampInterval candidateInterval;
	candidateInterval.start = interval.start;
	candidateInterval.finish = interval.finish;
	
	ve->lockEntry();
	bool is_success = ve->manager.LockWriteInterval(tid, candidateInterval);
	ve->unlockEntry();
	veAcc.release();

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

	VersionMap::accessor veAcc;
	if(all_versions.find(veAcc, key)){
		std::shared_ptr<VersionEntry> ve = veAcc->second;
		ve->lockEntry();
		ve->versions.insert(new_ts,value);
		ve->manager.CommitInterval(tid, old_ts, new_ts);
		ve->unlockEntry();
	}

	//GC
	//GarbageCollection(new_ts - (100 << 8));
	veAcc.release();
	return true;
}

bool VersionManager::PersistReadLock(TransactionId tid, Key key, Timestamp old_ts, Timestamp new_ts){
 	VersionMap::accessor veAcc;
 	if(all_versions.find(veAcc, key)){
 		std::shared_ptr<VersionEntry> ve = veAcc->second;
 		ve->lockEntry();
 		ve->manager.CommitInterval(tid, old_ts, new_ts);
 		ve->unlockEntry();
 	}
 	veAcc.release();
 	return true;
}

bool VersionManager::RemoveVersion(TransactionId tid, Key key, Timestamp old_ts){
	
	#ifdef DEBUG
		//std::cout<<"VersionManager: Remove Key:"<<key<<endl;
	#endif
	bool suss = true;
	VersionMap::accessor veAcc;
	if(all_versions.find(veAcc, key)){
		std::shared_ptr<VersionEntry> ve = veAcc->second;
		ve->lockEntry();
		suss &= ve->manager.RemoveLock(tid, old_ts);
		ve->unlockEntry();
	}
	veAcc.release();
	return suss;
}

bool VersionManager::GarbageCollection(Timestamp ts){
	#ifdef DEBUG
		std::cout<<"VersionManager: Garbage Collection:"<<ts<<std::endl;
	#endif
	bool suss = true;
	/*
	tbb::parallel_for(all_versions.range(), [&suss, ts](const VersionMap::range_type &r) {
    	for(VersionMap::iterator v = r.begin(); v != r.end(); v++){
    		v->second->versions.erase(ts);
			suss &= v->second->manager.GarbageCollection(ts);
    	}
	});
	*/
	const VersionMap::range_type &r = all_versions.range();
	for(VersionMap::iterator v = r.begin(); v != r.end(); v++){
		v->second->lockEntry();
		v->second->versions.erase(ts);
		suss &= v->second->manager.GarbageCollection(ts);
		v->second->unlockEntry();
	}
	return suss;
}

bool VersionManager::GetSize(int& version_len, int& lock_len){
	bool suss = true;
	/*
	tbb::parallel_for(all_versions.range(), [&version_len, &lock_len](const VersionMap::range_type &r){
		for(VersionMap::iterator v = r.begin(); v != r.end(); v++){
			version_len += v->second->versions.size();
			lock_len += v->second->manager.GetSize();
		}
	});
	*/
	const VersionMap::range_type &r = all_versions.range();
	for(VersionMap::iterator v = r.begin(); v != r.end(); v++){
		version_len += v->second->versions.size();
		lock_len += v->second->manager.GetSize();
	}
	return suss;
}
