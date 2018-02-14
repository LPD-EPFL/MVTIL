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

void Scheduler::HandleAbort(AbortReply& _return, const TransactionId tid){

	#ifdef DEBUG
		cout<<"Abort transaction tid:"<<tid<<" "<<endl;		
	#endif

	TxMap::accessor txAcc;
	if(pending_operations.find(txAcc, tid)){
		OperationEntry operations = txAcc->second;
		for(auto const& operation:operations){
			Key k = operation.first; 
			if(operation.second.first == LockOperation::WRITE)
			{
				#ifdef DEBUG
					cout<<"Remove write lock on key:"<<k<<" "<<endl;		
				#endif
				//Release lock on k
				data_locks.erase(k);
			}
			else if(operation.second.first == LockOperation::READ)
			{
				#ifdef DEBUG
					cout<<"Remove read lock on key:"<<k<<" "<<endl;		
				#endif

				Locks::accessor lockAcc;
				if(data_locks.find(lockAcc, k)){
					LockEntry& lockEntry = lockAcc->second;
					lockEntry.second.erase(tid);
				}
				lockAcc.release();
			}
		}
		pending_operations.erase(txAcc);
	}
	txAcc.release();
	_return.tid = tid;
	_return.state = OperationState::ABORT_OK;
}

void Scheduler::HandleCommit(CommitReply& _return, const TransactionId tid){
	
	TxMap::accessor txAcc;
	if(pending_operations.find(txAcc, tid)){
		OperationEntry operations = txAcc->second;
		for(auto const& operation:operations){
			Key k = operation.first;
			if(operation.second.first == LockOperation::WRITE)
			{
				#ifdef DEBUG
					cout<<"Remove write lock on key:"<<operation.first<<" "<<endl;		
				#endif
				DataMap::accessor dataAcc;
				if(!persist_versions.find(dataAcc, k)){
					persist_versions.insert(dataAcc, k);
				}
				dataAcc->second = operation.second.second;
				data_locks.erase(k);
			}
			else if(operation.second.first == LockOperation::READ)
			{
				#ifdef DEBUG
					cout<<"Remove read lock on key:"<<operation.first<<" "<<endl;		
				#endif
				Locks::accessor lockAcc;
				if(data_locks.find(lockAcc, k)){
					LockEntry& lockEntry = lockAcc->second;
					lockEntry.second.erase(tid);
				}
				lockAcc.release();
			}
		}
		pending_operations.erase(txAcc);
	}
	txAcc.release();
	_return.tid = tid;
	_return.state = OperationState::COMMIT_OK;
}

void Scheduler::HandleReadRequest(ReadReply& _return, const TransactionId tid, const Key& k, const bool isReadOnly){
	#ifdef DEBUG
		cout<<"Try to obtain read lock on key:"<<k<<" tid:"<<tid<<" "<<endl;		
	#endif

	int numRetries = 0;
	std::chrono::time_point<std::chrono::high_resolution_clock> tStart = std::chrono::high_resolution_clock::now();
	_return.key = k;
	_return.tid = tid;
	while(true){
		bool suss = false;
		Locks::accessor lockAcc;
		if(data_locks.find(lockAcc, k)){
			LockEntry& lockEntry = lockAcc->second;
			if(lockEntry.first == LockOperation::READ){
				#ifdef DEBUG
					cout<<"Share read lock with other transactions on key:"<<k<<" "<<endl;		
				#endif
				lockEntry.second.insert(tid);
				suss = true;
			}
		}
		else
		{
			#ifdef DEBUG
				cout<<"Create the read lock on key:"<<k<<" "<<endl;		
			#endif
			std::unordered_set<TransactionId> txVec;
			txVec.insert(tid);
			data_locks.insert(lockAcc,k);
			lockAcc->second = std::make_pair(LockOperation::READ,txVec);
			suss = true;
		}
		lockAcc.release();

		//Here
		if(suss){
			DataMap::accessor dataAcc;
			if(persist_versions.find(dataAcc, k))
			{
				_return.state = OperationState::R_LOCK_SUCCESS;
				_return.value = dataAcc->second;
			}
			else
			{
				_return.state = OperationState::FAIL_NO_VERSION;
				_return.value = "";
			}
			dataAcc.release();
			break;
		}

		auto now = std::chrono::high_resolution_clock::now();
		//!isReadOnly()
		if(numRetries > read_retry && timeout(tStart,now)){
			#ifdef DEBUG
				cout<<"Read lock failed on key:"<<k<<" "<<endl;		
			#endif
			_return.state = OperationState::FAIL_TIMEOUT;
			_return.value = "";
			return;
		}

		std::this_thread::yield();
		//std::this_thread::sleep_for(PAUSE_LENGTH);
        numRetries++;
	}

	//Add key,value into pending version
	TxMap::accessor txAcc;
	if(!pending_operations.find(txAcc, tid)){
		pending_operations.insert(txAcc, tid);
	}
	txAcc->second[k] = std::make_pair(LockOperation::READ,"");
}

void Scheduler::HandleWriteRequest(WriteReply& _return, const TransactionId tid, const Key& k, const Value& v){

	#ifdef DEBUG
		cout<<"Try to obtain write lock on key:"<<k<<", value:"<<v<<", tid:"<<tid<<" "<<endl;		
	#endif

	int numRetries = 0;
	std::chrono::time_point<std::chrono::high_resolution_clock> tStart = std::chrono::high_resolution_clock::now();
	_return.key = k;
	_return.tid = tid;

	TxMap::accessor txAcc;
	//If this transaction wrote this key before
	if(pending_operations.find(txAcc, tid)){		
		OperationEntry& operations = txAcc->second;
		auto it = operations.find(k);
		if(it != operations.end() && it->second.first == LockOperation::WRITE)
		{
			it->second.second = v;
			_return.state = OperationState::W_LOCK_SUCCESS;
			return;
		}
	}
	txAcc.release();

	while(true){
		bool suss = false;
		Locks::accessor lockAcc;
		if(!data_locks.find(lockAcc, k)){
			//If there doesn't exist any lock on this key
			#ifdef DEBUG
				cout<<"No lock on this key, so create write lock on:"<<k<<" "<<endl;		
			#endif
			data_locks.insert(lockAcc, k);
			std::unordered_set<TransactionId> tranx_vec;
			tranx_vec.insert(tid);
			lockAcc->second = std::make_pair(LockOperation::WRITE,tranx_vec);
			suss = true;
		}
		else{
			//If there exists locks on this key
			LockEntry& lockEntry = lockAcc->second;
			auto& tranx_vec = lockEntry.second;
			if(tranx_vec.size() == 0){
				lockEntry.first = LockOperation::WRITE;
				lockEntry.second.insert(tid);
				suss = true;
			}
			else if(tranx_vec.size() == 1 && *(tranx_vec.cbegin()) == tid){
				#ifdef DEBUG
					cout<<"Convert read lock into write lock:"<<k<<" "<<endl;		
				#endif
				lockEntry.first = LockOperation::WRITE;
				suss = true;
			}
		}
		lockAcc.release();

		if(suss){
			_return.state = OperationState::W_LOCK_SUCCESS;
			break;
		}

		auto now = std::chrono::high_resolution_clock::now();
		if ((numRetries > MIN_NUM_RETRIES) && (timeout(tStart, now))) {
			#ifdef DEBUG
				cout<<"Write lock failed on key:"<<k<<" "<<endl;		
			#endif
            _return.state = OperationState::FAIL_TIMEOUT;
            return;
        }

        std::this_thread::yield();
        //std::this_thread::sleep_for(PAUSE_LENGTH);
        numRetries++;
	}

	if(!pending_operations.find(txAcc, tid)){
		pending_operations.insert(txAcc, tid);
	}
	txAcc->second[k] = std::make_pair(LockOperation::WRITE,v);
}

void Scheduler::GetSize(int& lock_len){
	lock_len = data_locks.size();
}
