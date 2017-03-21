#include "Scheduler.h"

void Scheduler::HandleReadRequest(ReadReply& _return, const TransactionId tid,
 const TimestampInterval& interval, const Key& k){
 	int numRetries = 0;
	//bool pending_exists=false;
	std::chrono::time_point<std::chrono::high_resolution_clock> tStart = std::chrono::high_resolution_clock::now();
	LockInfo lockInfo;
	Value value;
	while(true){

		#ifdef DEBUG
        	std::cout<<"Scheduler: Read Operation, tid:"<< tid <<",key:" << k << ",interval:["<<interval.start<<","<<interval.finish<<"]"<<endl;
    	#endif

		version_manager.TryReadLock(tid, k,interval,lockInfo);
		if(lockInfo.state == OperationState::R_LOCK_SUCCESS){
			value = lockInfo.version->value;
			break;
		}
		else if(lockInfo.state == OperationState::FAIL_NO_VERSION){
			_return.tid = tid;
            _return.state = lockInfo.state;
            _return.key = k;
            _return.value = "";
            _return.interval = interval;
            return;
		}

		//If read operation wait a pending write.
		auto now = std::chrono::high_resolution_clock::now();
		if ((numRetries > MIN_NUM_RETRIES) || (timeout(tStart, now))) {
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

        std::this_thread::yield();
        //std::this_thread::sleep_for(PAUSE_LENGTH);
        numRetries++;
	}

	_return.tid = tid;
    _return.state = lockInfo.state;
    _return.key = k;
    _return.value = value;
    _return.interval = lockInfo.locked;
    //_return.potential = lockInfo.potential;
    return;
}

void Scheduler::HandleWriteRequest(WriteReply& _return, const TransactionId tid, 
	const TimestampInterval& interval, const Key& k, const Value& value){

	#ifdef DEBUG
    	std::cout<<"Scheduler: Write Operation, tid:"<< tid <<",key:" << k << ",interval:["<<interval.start<<","<<interval.finish<<"]"<<endl;
	#endif

	//????
	//cuckoohash_map<Key, std::pair<Value,TimestampInterval>, CityHasher<Key>> write_set;
	std::unordered_map<Key, std::pair<Value,TimestampInterval>> write_set;
	if(pendingWriteSets.find(tid, write_set)){
		//std::pair<Value,TimestampInterval> value_set;
		if(write_set.count(k) > 0){
			auto& value_time = write_set[k];
			value_time.first = value;
		}
		// if(write_set.find(k,value_set)){
		// 	value_set.first = value;
		// }
		return;
	}

	LockInfo lockInfo;
    version_manager.TryWriteLock(tid, k, interval, lockInfo);

    if (lockInfo.state != OperationState::W_LOCK_SUCCESS) {
        //Todo Abort Transacrion
        //AbortTransaction(tid);
        _return.tid = tid;
        _return.state = lockInfo.state;
        _return.interval = lockInfo.locked;
        //_return.potential = lockInfo.potential;
        _return.key = k;
        return;
    }
        
    _return.tid = tid;
    _return.state = lockInfo.state;
    _return.interval = lockInfo.locked;
    _return.key = k;

    //cuckoohash_map<Key, std::pair<Value,TimestampInterval>, CityHasher<Key>> valueEntry;
    std::unordered_map<Key, std::pair<Value,TimestampInterval>> valueEntry; 

    auto value_pair = make_pair(value,lockInfo.locked);
    //valueEntry.insert(k, value_pair);
    valueEntry[k] = value_pair;
    pendingWriteSets.insert(tid, valueEntry);
    return;
}

void Scheduler::HandleAbort(AbortReply& _return, const TransactionId tid){
	// cuckoohash_map<Key, std::pair<Value,TimestampInterval>, CityHasher<Key>> write_set;
	// if(!pendingWriteSets.find(tid, write_set)){
	// 	_return.state = OperationState::WRITES_NOT_FOUND;
 //        _return.tid = tid;
 //        return;
	// }
	// bool suss = true;
	// for(const auto& item:write_set.lock_table()){
	// 	suss &= version_manager.RemoveVersion(item.first,item.second.second);
	// }

	#ifdef DEBUG
    	std::cout<<"Scheduler: Abort Operation, tid:"<< tid <<endl;
	#endif

	std::unordered_map<Key, std::pair<Value,TimestampInterval>> write_set;
	if(!pendingWriteSets.find(tid, write_set)){
		_return.state = OperationState::WRITES_NOT_FOUND;
        _return.tid = tid;
        return;
	}
	bool suss = true;
	for(const auto& item:write_set){
		suss &= version_manager.RemoveVersion(item.first,item.second.second);
	}
	
	//Not sure
	write_set.clear();
	pendingWriteSets.erase(tid);
	if(suss){
		_return.state = OperationState::ABORT_OK;
	}
	else{
		_return.state = OperationState::ERROR;
	}
    _return.tid = tid;
	return;
}

void Scheduler::HandleCommit(CommitReply& _return, const TransactionId tid, const Timestamp ts){
	//cuckoohash_map<Key, std::pair<Value,TimestampInterval>, CityHasher<Key>> write_set;
	// bool suss = true;
	// if(pendingWriteSets.find(tid, write_set)){
	// 	for(const auto& item:write_set.lock_table()){
	// 		suss &= version_manager.UpdateAndPersistVersion(item.first,item.second.first,ts);
	// 	}
	// }
	//Not sure
	//write_set.clear();
	#ifdef DEBUG
    	std::cout<<"Scheduler: Commit Operation, tid:"<< tid <<",timestamp:"<< ts <<endl;
	#endif

	std::unordered_map<Key, std::pair<Value,TimestampInterval>> write_set;
	bool suss = true;
	if(pendingWriteSets.find(tid, write_set)){
		for(const auto& item:write_set){
			suss &= version_manager.UpdateAndPersistVersion(item.first,item.second.first,ts);
		}
	}

	pendingWriteSets.erase(tid);
	if(suss){
		_return.state = OperationState::ABORT_OK;
	}
	else{
		_return.state = OperationState::ERROR;
	}
    _return.tid = tid;
	return;
}


void Scheduler::HandleFreezeReadRequest(ReadReply& _return, const TransactionId tid, const TimestampInterval& interval, const Key& k)
{

}