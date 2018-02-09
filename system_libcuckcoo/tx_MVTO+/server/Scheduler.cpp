#include "Scheduler.h"

void Scheduler::HandleAbort(AbortReply& _return, const TransactionId tid, const Timestamp ts){
	#ifdef DEBUG
		std::cout<<"Scheduler: Abort Operation, tid:"<< tid <<endl;
	#endif	

	std::shared_ptr<WriteEntry> writeEntry;
	if(pendingWrite.find(tid, writeEntry)){
		for(const Key& k: *writeEntry){
			std::shared_ptr<VersionList> versionList;
			if(versions.find(k, versionList)){
				lockSet.lock(k);
				versionList->remove(ts);
				lockSet.unlock(k);
			}

		}
		pendingWrite.erase(tid);
	}

	std::shared_ptr<ReadEntry> readEntry;
	if(pendingRead.find(tid, readEntry)){
		for(const auto& it: *readEntry){
			Key k = it.first;
			std::shared_ptr<VersionList> versionList;
			if(versions.find(k, versionList)){
				Version* prev;
				lockSet.lock(k);
				Version* curr = versionList->find(it.second, prev);
				curr->value.read_time.erase(ts);
				lockSet.unlock(k);
			}
		}
		pendingRead.erase(tid);
	}
	_return.tid = tid;
	_return.state = OperationState::ABORT_OK;
}

void Scheduler::HandleCommit(CommitReply& _return, const TransactionId tid, const Timestamp ts){
	#ifdef DEBUG
		std::cout<<"Scheduler: Commit Operation, tid:"<< tid <<endl;
	#endif

	std::shared_ptr<WriteEntry> writeEntry;
	if(pendingWrite.find(tid, writeEntry)){
		for(const Key& k: *writeEntry){
			std::shared_ptr<VersionList> versionList;
			if(versions.find(k, versionList)){
				Version* prev;
				lockSet.lock(k);
				Version* curr = versionList->find(ts, prev);
				curr->value.version_state = VersionState::COMMITTED;
				lockSet.unlock(k);
			}
		}
		pendingWrite.erase(tid);
	}

	pendingRead.erase(tid);
	_return.tid = tid;
	_return.state = OperationState::COMMIT_OK;
}

void Scheduler::HandleReadRequest(ReadReply& _return, const TransactionId tid, const Timestamp ts, const Key& k){
	
	#ifdef DEBUG
		std::cout<<"Scheduler: Read Operation, tid:"<< tid <<endl;
	#endif
	_return.tid = tid;
	_return.ts = ts;
	_return.key = k;
	std::chrono::time_point<std::chrono::high_resolution_clock> tStart = std::chrono::high_resolution_clock::now();
	int numRetries = 0;
	//Find whether there is a version list for this key
	while(true){

		bool suss = false;
		std::shared_ptr<VersionList> versionList;
		if(versions.find(k, versionList)){
			#ifdef DEBUG
				std::cout<<"Scheduler: Read Operation, Found"<<endl;
			#endif
			Version *prev;
			lockSet.lock(k);
			versionList->find(ts, prev);
			VersionState state = prev->value.version_state;
			if(state == VersionState::COMMITTED){
				prev->value.read_time.insert(ts);
				suss = true;
			}
			lockSet.unlock(k);
			if(suss){
				//Add to read set
				std::shared_ptr<ReadEntry> readEntry;
				if(!pendingRead.find(tid, readEntry)){
					readEntry = std::make_shared<ReadEntry>();
					pendingRead.insert(tid, readEntry);
				}
				(*readEntry)[k] = prev->key;
				//Results
				_return.value = prev->value.version_value;
				_return.state = OperationState::R_SUCCESS;
				return;
			}
		}
		else{
			#ifdef DEBUG
				std::cout<<"Scheduler: Read Operation, Found"<<endl;
			#endif
			//No version for this key has been created before.
			_return.state = OperationState::FAIL_NO_VERSION;
			_return.value = "";
			return;
		}

		//Otherwise the latest version is still in pending commit.
		auto now = std::chrono::high_resolution_clock::now();
		if ((numRetries > read_retry) && (timeout(tStart, now))) {
			_return.state = OperationState::R_FAIL_TIMEOUT;
			_return.value = "";
			return;
		}

		std::this_thread::yield();
		numRetries++;
	}
}

void Scheduler::HandleWriteRequest(WriteReply& _return, const TransactionId tid, const Timestamp ts, const Key& k, const Value& v){
	
	#ifdef DEBUG
		std::cout<<"Scheduler: Write Operation, tid:"<< tid <<",key:" << k <<endl;
	#endif	

	_return.tid = tid;
	_return.ts = ts;
	_return.key = k;

	std::shared_ptr<WriteEntry> writeEntry;
	if(pendingWrite.find(tid, writeEntry)){
		if(writeEntry->find(k) != writeEntry->end()){
			std::shared_ptr<VersionList> versionList;
			if(versions.find(k, versionList)){
				Version* prev;
				lockSet.lock(k);
				Version* curr = versionList->find(ts, prev);
				curr->value.version_value = v;
				lockSet.unlock(k);
			}
			_return.state = OperationState::W_SUCCESS;
			return;
		}
	}

	std::shared_ptr<VersionList> versionList;
	if(versions.find(k, versionList)){
		Version *prev;
		lockSet.lock(k);
		Version *next = versionList->find(ts, prev);
		std::set<Timestamp> readTsSet = prev->value.read_time;
		if(readTsSet.size() > 0 && *(readTsSet.rbegin()) > ts){
			_return.state = OperationState::W_FAIL_LATE;
		}
		else
		{
			std::set<Timestamp> versionWrite;
			versionList->insert(ts, VersionEntry(v, VersionState::PENDING, versionWrite));
			/*
			Version* insert_v = new Version(ts, VersionEntry(v, VersionState::PENDING, versionWrite));
			int level = versionList->GetRandomLevel();
			for(int i=0;i<prev->top_level;i++){
				prev->next[i] = insert_v;
			}
			for(int i=0;i<level;i++){
				insert_v->next[i] = next;
			}
			*/
			_return.state = OperationState::W_SUCCESS;
		}
		lockSet.unlock(k);
	}
	else{
		versionList = std::make_shared<VersionList>(MIN_TIMESTAMP,MAX_TIMESTAMP);
		std::set<Timestamp> readTsSet;
		versionList->insert(ts, VersionEntry(v,VersionState::PENDING,readTsSet));
		versions.insert(k, versionList);
		_return.state = OperationState::W_SUCCESS;
	}

	if(_return.state == OperationState::W_SUCCESS)
	{
		std::shared_ptr<WriteEntry> writeEntry;
		if(!pendingWrite.find(tid, writeEntry)){
			writeEntry = std::make_shared<WriteEntry>();
			pendingWrite.insert(tid, writeEntry);
		}
		writeEntry->insert(k);
	}	
}

void Scheduler::GetSize(int& version_len){
	version_len = 0;
	{
		auto vl = versions.lock_table();
		for (const auto &ve : vl) {
			version_len += ve.second->size();
		}
	}
}
