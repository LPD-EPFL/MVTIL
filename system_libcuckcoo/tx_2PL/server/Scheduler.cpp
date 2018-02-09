#include "Scheduler.h"

void Scheduler::HandleAbort(AbortReply& _return, const TransactionId tid){

	#ifdef DEBUG
		cout<<"Abort transaction tid:"<<tid<<" "<<endl;		
	#endif

	std::shared_ptr<OperationEntry> operations;
	if(pending_operations.find(tid, operations)){
		for(auto const& operation:*operations){
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
				
				LockEntry lockEntry;
				if(data_locks.find(k, lockEntry)){
					lockSet.lock(k);
					lockEntry.second->erase(tid);
					lockSet.unlock(k);
				}
			}
		}
		pending_operations.erase(tid);
	}
	_return.tid = tid;
	_return.state = OperationState::ABORT_OK;
}

void Scheduler::HandleCommit(CommitReply& _return, const TransactionId tid){
	
	std::shared_ptr<OperationEntry> operations;
	if(pending_operations.find(tid, operations)){
		for(auto const& operation:*operations){
			Key k = operation.first;
			Value val = operation.second.second;
			if(operation.second.first == LockOperation::WRITE)
			{
				#ifdef DEBUG
					cout<<"Remove write lock on key:"<<operation.first<<" "<<endl;		
				#endif

				std::shared_ptr<Value> val_ptr;
				if(!persist_versions.find(k, val_ptr)){
					val_ptr = std::make_shared<Value>();
					persist_versions.insert(k, val_ptr);
				}
				*val_ptr = val;
				data_locks.erase(k);
			}
			else if(operation.second.first == LockOperation::READ)
			{
				#ifdef DEBUG
					cout<<"Remove read lock on key:"<<operation.first<<" "<<endl;		
				#endif
				LockEntry lockEntry;
				if(data_locks.find(k, lockEntry)){
					lockSet.lock(k);
					lockEntry.second->erase(tid);
					lockSet.unlock(k);
				}
			}
		}
		pending_operations.erase(tid);
	}
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
		LockEntry lockEntry;
		if(data_locks.find(k, lockEntry)){
			if(lockEntry.first == LockOperation::READ){
				#ifdef DEBUG
					cout<<"Share read lock with other transactions on key:"<<k<<" "<<endl;		
				#endif
				lockSet.lock(k);
				lockEntry.second->insert(tid);
				lockSet.unlock(k);
				suss = true;
			}
		}
		else
		{
			#ifdef DEBUG
				cout<<"Create the read lock on key:"<<k<<" "<<endl;		
			#endif
			lockEntry.first = LockOperation::READ;
			lockEntry.second = std::make_shared<std::unordered_set<TransactionId>>();
			lockEntry.second->insert(tid);
			data_locks.insert(k,lockEntry);
			suss = true;
		}

		//Here
		if(suss){
			std::shared_ptr<Value> val_ptr;
			if(persist_versions.find(k, val_ptr))
			{
				_return.state = OperationState::R_LOCK_SUCCESS;
				_return.value = *val_ptr;
			}
			else
			{
				_return.state = OperationState::FAIL_NO_VERSION;
				_return.value = "";
			}
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
	std::shared_ptr<OperationEntry> operations;
	if(!pending_operations.find(tid, operations)){
		operations = std::make_shared<OperationEntry>();
		pending_operations.insert(tid, operations);
	}
	(*operations)[k] = std::make_pair(LockOperation::READ,"");

}

void Scheduler::HandleWriteRequest(WriteReply& _return, const TransactionId tid, const Key& k, const Value& v){

	#ifdef DEBUG
		cout<<"Try to obtain write lock on key:"<<k<<", value:"<<v<<", tid:"<<tid<<" "<<endl;		
	#endif

	int numRetries = 0;
	std::chrono::time_point<std::chrono::high_resolution_clock> tStart = std::chrono::high_resolution_clock::now();
	_return.key = k;
	_return.tid = tid;

	//If this transaction wrote this key before
	std::shared_ptr<OperationEntry> operations;
	if(pending_operations.find(tid, operations)){
		auto it = operations->find(k);
		if(it != operations->end() && it->second.first == LockOperation::WRITE)
		{
			it->second.second = v;
			_return.state = OperationState::W_LOCK_SUCCESS;
			return;
		}
	}

	while(true){
		bool suss = false;
		LockEntry lockEntry;
		if(data_locks.find(k, lockEntry)){
			//If there exists locks on this key
			lockSet.lock(k);
			auto tx_vec = lockEntry.second;
			if(tx_vec->size() == 0){
				lockEntry.first = LockOperation::WRITE;
				tx_vec->insert(tid);
				suss = true;
			}
			else if(tx_vec->size() == 1 && *(tx_vec->cbegin()) == tid){
				#ifdef DEBUG
					cout<<"Convert read lock into write lock:"<<k<<" "<<endl;		
				#endif
				lockEntry.first = LockOperation::WRITE;
				suss = true;
			}
			lockSet.unlock(k);
		}
		else
		{
			//If there doesn't exist any lock on this key
			#ifdef DEBUG
				cout<<"No lock on this key, so create write lock on:"<<k<<" "<<endl;		
			#endif
			lockEntry.first = LockOperation::WRITE;
			lockEntry.second = std::make_shared<unordered_set<TransactionId>>();
			lockEntry.second->insert(tid);
			data_locks.insert(k, lockEntry);
			suss = true;
		}

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

	std::shared_ptr<OperationEntry> w_operations;
	if(!pending_operations.find(tid, w_operations)){
		w_operations = std::make_shared<OperationEntry>();
		pending_operations.insert(tid, w_operations);
	}
	(*w_operations)[k] = std::make_pair(LockOperation::WRITE,v);
}

void Scheduler::GetSize(int& lock_len){
	lock_len = data_locks.size();
}
