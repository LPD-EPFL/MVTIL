/*   
 *   File: Scheduler.cpp
 *   Author: Junxiong Wang <junxiong.wang@epfl.ch>
 *   Description: 
 *   Scheduler.cpp is part of MVTLSYS
 *
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
 */#include "Scheduler.h"

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

	std::shared_ptr<ReadItem> readItem;
	if(!pendingReadSets.find(tid, readItem)){
		readItem = std::make_shared<ReadItem>();
		pendingReadSets.insert(tid, readItem);
	}
	(*readItem)[k] = lockInfo.locked.lock_start;
	
	/*
	pendingReadSets.update_fn(tid, [=](std::shared_ptr<ReadItem> &read_it){
		(*read_it)[k] = lockInfo.locked.lock_start;
	});
	*/
	return;
}

void Scheduler::HandleWriteRequest(WriteReply& _return, const TransactionId tid, 
	const TimestampInterval& interval, const Key& k, const Value& value){

	#ifdef DEBUG
		std::cout<<endl<<"Scheduler: Write Operation, tid:"<< tid <<",key:" << k << ",interval:["<<interval.start<<","<<interval.finish<<"]"<<endl;
	#endif
	
	_return.tid = tid;
	_return.key = k;

	std::shared_ptr<WriteItem> writeItem;
	if(pendingWriteSets.find(tid, writeItem)){
		auto it = writeItem->find(k);
		if(it != writeItem->end()){
			it -> second.first = value;
			_return.state = OperationState::W_LOCK_SUCCESS;
			_return.interval = interval;
			return;
		}
	}

	LockInfo lockInfo;
	version_manager.TryWriteLock(tid, k, interval, lockInfo);
	_return.state = lockInfo.state;
	if (lockInfo.state != OperationState::W_LOCK_SUCCESS) {
		return;
	}

	_return.interval = lockInfo.locked;	
	if(!pendingWriteSets.find(tid, writeItem)){
		writeItem = std::make_shared<WriteItem>();
		pendingWriteSets.insert(tid, writeItem);
	}
	(*writeItem)[k] = std::make_pair(value, lockInfo.locked.start);
	/*
	pendingWriteSets.update_fn(tid, [=](std::shared_ptr<WriteItem> &write_it){
		(*write_it)[k] = std::make_pair(value, lockInfo.locked.start);
	});
	*/
	return;
}

void Scheduler::HandleAbort(AbortReply& _return, const TransactionId tid){

	#ifdef DEBUG
		std::cout<<endl<<"Scheduler: Abort Operation, tid:"<< tid <<endl;
	#endif

	bool suss = true;
	std::shared_ptr<WriteItem> writeItem;
	if(pendingWriteSets.find(tid, writeItem)){
		for(const auto& item:*writeItem){
			suss &= version_manager.RemoveVersion(tid, item.first, item.second.second);
		}
		pendingWriteSets.erase(tid);
	}
	
	std::shared_ptr<ReadItem> readItem;
	if(pendingReadSets.find(tid, readItem)){
		for(const auto& item:*readItem){
			suss &= version_manager.RemoveVersion(tid, item.first, item.second);
		}
		pendingReadSets.erase(tid);
	}

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
	std::shared_ptr<WriteItem> writeItem;
	if(pendingWriteSets.find(tid, writeItem)){
		for(const auto& item: *writeItem){
			suss &= version_manager.UpdateAndPersistVersion(tid, item.first, item.second.first, item.second.second, ts);
		}
		pendingWriteSets.erase(tid);
	}

	std::shared_ptr<ReadItem> readItem;
	if(pendingReadSets.find(tid, readItem)){
		for(const auto& item: *readItem){
			suss &= version_manager.PersistReadLock(tid, item.first, item.second, ts);
		}
		pendingReadSets.erase(tid);
	}

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
