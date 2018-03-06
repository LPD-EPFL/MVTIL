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
 */

#include "Scheduler.h"

void Scheduler::HandleAbort(AbortReply& _return, const TransactionId tid, const Timestamp ts){
	#ifdef DEBUG
    	std::cout<<"Scheduler: Abort Operation, tid:"<< tid <<endl;
	#endif	

    WriteMap::accessor writeAcc;
	if(pendingWrite.find(writeAcc, tid)){
		std::unordered_set<Key>& writeSet = writeAcc->second;
		for(const Key& k: writeSet){
			VersionMap::accessor versionAcc;
			if(versions.find(versionAcc, k)){
				std::shared_ptr<VersionList> versionList = versionAcc->second;
				lockSet.lock(k);
				versionList->remove(ts);
				lockSet.unlock(k);
			}
		}
		pendingWrite.erase(writeAcc);
	}
	writeAcc.release();

	ReadMap::accessor readAcc;
	if(pendingRead.find(readAcc, tid)){
		std::unordered_map<Key, Timestamp>& readSet = readAcc->second;
		for(const auto& it: readSet){
			VersionMap::accessor versionAcc;
			if(versions.find(versionAcc, it.first)){
				std::shared_ptr<VersionList> versionList = versionAcc->second;
				Version* prev;
				lockSet.lock(it.first);
				Version* curr = versionList->find(it.second, prev);
				curr->value.read_time.erase(ts);
				lockSet.unlock(it.first);
			}
		}
		pendingRead.erase(readAcc);
	}
	readAcc.release();
	_return.tid = tid;
	_return.state = OperationState::ABORT_OK;
}

void Scheduler::HandleCommit(CommitReply& _return, const TransactionId tid, const Timestamp ts){
	#ifdef DEBUG
    	std::cout<<"Scheduler: Commit Operation, tid:"<< tid <<endl;
	#endif

	WriteMap::accessor writeAcc;
	if(pendingWrite.find(writeAcc, tid)){
		unordered_set<Key>& writeSet = writeAcc->second;
		for(const Key& k: writeSet){
			VersionMap::accessor versionAcc;
			if(versions.find(versionAcc, k)){
				std::shared_ptr<VersionList> versionList = versionAcc->second;
				Version* prev;
				lockSet.lock(k);
				Version* curr = versionList->find(ts, prev);
				curr->value.version_state = VersionState::COMMITTED;
				lockSet.unlock(k);
			}
		}
		pendingWrite.erase(writeAcc);
	}
	writeAcc.release();
	ReadMap::accessor readAcc;
	if(pendingRead.find(readAcc, tid)){
		pendingRead.erase(readAcc);
	}
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
		VersionMap::accessor versionAcc;
		if(versions.find(versionAcc, k)){
			#ifdef DEBUG
				std::cout<<"Scheduler: Read Operation, Found"<<endl;
			#endif
			std::shared_ptr<VersionList> versionList = versionAcc->second;
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
				ReadMap::accessor readAcc;
				if(!pendingRead.find(readAcc, tid)){
					pendingRead.insert(readAcc, tid);
				}
				readAcc->second[k] = prev->key;
				readAcc.release();
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
		versionAcc.release();

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

	WriteMap::accessor writeAcc;
	if(pendingWrite.find(writeAcc, tid)){
		std::unordered_set<Key>& writeKey = writeAcc->second;
		if(writeKey.find(k) != writeKey.end()){
			VersionMap::accessor versionAcc;
			if(versions.find(versionAcc, k)){
				std::shared_ptr<VersionList> versionList = versionAcc->second;
				Version* prev;
				Version* curr = versionList->find(ts, prev);
				curr->value.version_value = v;
			}
			_return.state = OperationState::W_SUCCESS;
			return;
		}
	}
	writeAcc.release();
	
	VersionMap::accessor versionAcc;
	if(versions.find(versionAcc, k)){
		std::shared_ptr<VersionList> versionList = versionAcc->second;
		lockSet.lock(k);
		Version *prev;
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
		versions.insert(versionAcc, k);
		std::shared_ptr<VersionList> versionList = std::make_shared<VersionList>(MIN_TIMESTAMP,MAX_TIMESTAMP);
		std::set<Timestamp> readTsSet;
		versionList->insert(ts, VersionEntry(v,VersionState::PENDING,readTsSet));
		versionAcc->second = versionList;
		_return.state = OperationState::W_SUCCESS;
	}
	versionAcc.release();

	if(_return.state == OperationState::W_SUCCESS)
	{
		WriteMap::accessor writeAcc;
		if(!pendingWrite.find(writeAcc, tid)){
			pendingWrite.insert(writeAcc, tid);
		}
		writeAcc->second.insert(k);
		writeAcc.release();
	}	
}

void Scheduler::GetSize(int& version_len){
	version_len = 0;
	const VersionMap::range_type &r = versions.range();
	for(VersionMap::iterator v = r.begin(); v != r.end(); v++){
		version_len += v->second->size();
	}
}
