/*   
 * Copyright (c) 2017 Junxiong Wang <junxiong.wang@epfl.ch>,
 *                Tudor David <tudor.david@epfl.ch>
 *                Distributed Programming Lab (LPD), EPFL
 *
 * MVTLSYS is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, version 2
 * of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include "Scheduler.h"

void Scheduler::HandleReadRequest(ReadReply& _return, const TransactionId tid,
 const TimestampInterval& interval, const Key& k){
	
	#ifdef DEBUG
		std::cout<<endl<<"Scheduler: Read Operation, tid:"<< tid <<",key:" << k << ",interval:["<<interval.start<<","<<interval.finish<<"]"<<endl;
	#endif

	int numRetries = 0;
	std::chrono::time_point<std::chrono::high_resolution_clock> tStart = std::chrono::high_resolution_clock::now();
	LockInfo lockInfo;
	_return.tid = tid;
	_return.key = k;

	while(true){
		version_manager.TryReadLock(tid, k, interval, lockInfo);
		if(lockInfo.state == OperationState::R_LOCK_SUCCESS){
			_return.state = lockInfo.state;
			_return.value = lockInfo.version->value;
			_return.interval = lockInfo.locked;
			break;
		}
		else if(lockInfo.state == OperationState::FAIL_NO_VERSION){
			_return.state = lockInfo.state;
			_return.value = "";
			_return.interval = lockInfo.locked;
			break;
		}

		//If read operation wait a pending write.
		auto now = std::chrono::high_resolution_clock::now();
		if ((numRetries > retry) && (timeout(tStart, now))) {
			_return.state = lockInfo.state; //should be FAIL_PENDING_VERSION
			_return.value = "";
			return;
		}

		std::this_thread::yield();
		numRetries++;
	}

	ReadMap::accessor readAcc;
	if(!pendingReadSets.find(readAcc, tid)){
		pendingReadSets.insert(readAcc, tid);
	}
	readAcc->second[k] = lockInfo.locked.lock_start;
	readAcc.release();
	return;
}

void Scheduler::HandleWriteRequest(WriteReply& _return, const TransactionId tid, 
	const TimestampInterval& interval, const Key& k, const Value& value){

	#ifdef DEBUG
		std::cout<<endl<<"Scheduler: Write Operation, tid:"<< tid <<",key:" << k << ",interval:["<<interval.start<<","<<interval.finish<<"]"<<endl;
	#endif
	
	_return.tid = tid;
	_return.key = k;

	WriteMap::accessor writeAcc;
	if(pendingWriteSets.find(writeAcc, tid)){
		std::unordered_map<Key, std::pair<Value, Timestamp>>& writeSet = writeAcc->second;
		auto it = writeSet.find(k);
		if(it != writeSet.end()){
			it -> second.first = value;
			_return.state = OperationState::W_LOCK_SUCCESS;
			_return.interval = interval;
			return;
		}
	}
	writeAcc.release();

	LockInfo lockInfo;
	version_manager.TryWriteLock(tid, k, interval, lockInfo);
	_return.state = lockInfo.state;
	if (lockInfo.state != OperationState::W_LOCK_SUCCESS) {
		return;
	}

	_return.interval = lockInfo.locked;	
	WriteMap::accessor insertAcc;
	if(!pendingWriteSets.find(insertAcc, tid)){
		pendingWriteSets.insert(insertAcc, tid);
	}
	insertAcc->second[k] = std::make_pair(value, lockInfo.locked.start);
	insertAcc.release();
	return;
}

void Scheduler::HandleAbort(AbortReply& _return, const TransactionId tid){

	#ifdef DEBUG
		std::cout<<endl<<"Scheduler: Abort Operation, tid:"<< tid <<endl;
	#endif

	WriteMap::accessor writeAcc;
	bool suss = true;
	if(pendingWriteSets.find(writeAcc, tid)){
		std::unordered_map<Key, std::pair<Value, Timestamp>>& writeSet = writeAcc->second;
		for(const auto& item:writeSet){
			suss &= version_manager.RemoveVersion(tid, item.first, item.second.second);
		}
		pendingWriteSets.erase(writeAcc);
	}
	writeAcc.release();
	
	ReadMap::accessor readAcc;
	if(pendingReadSets.find(readAcc, tid)){
		std::unordered_map<Key, Timestamp>& readSet = readAcc->second;
		for(const auto& item:readSet){
			suss &= version_manager.RemoveVersion(tid, item.first, item.second);
		}
		pendingReadSets.erase(readAcc);
	}
	readAcc.release();

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

	#ifdef DEBUG
		std::cout<<endl<<"Scheduler: Commit Operation, tid:"<< tid <<",timestamp:"<< ts <<endl;
	#endif

	bool suss = true;
	WriteMap::accessor writeAcc;
	if(pendingWriteSets.find(writeAcc, tid)){
		std::unordered_map<Key, std::pair<Value, Timestamp>>& writeSet = writeAcc->second;
		for(const auto& item:writeSet){
			suss &= version_manager.UpdateAndPersistVersion(tid,item.first,item.second.first,item.second.second,ts);
		}
		pendingWriteSets.erase(writeAcc);
	}
	writeAcc.release();

	ReadMap::accessor readAcc;
	if(pendingReadSets.find(readAcc, tid)){
		std::unordered_map<Key,Timestamp> readSet = readAcc->second;
		for(const auto& readIt:readSet){
			suss &= version_manager.PersistReadLock(tid,readIt.first,readIt.second,ts);
		}
		pendingReadSets.erase(readAcc);
	}
	readAcc.release();

	if(suss){
		_return.state = OperationState::COMMIT_OK;
	}
	else{
		_return.state = OperationState::ERROR;
	}
	_return.tid = tid;
	return;
}

void Scheduler::GarbageCollection(GCReply& _return, const Timestamp ts){
	version_manager.GarbageCollection(ts);
}

void Scheduler::GetSize(int& version_len, int& lock_len){
	version_manager.GetSize(version_len, lock_len);
}
