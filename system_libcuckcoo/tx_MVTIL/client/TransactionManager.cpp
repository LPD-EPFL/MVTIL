
#include "TransactionManager.h"

TransactionManager::TransactionManager(int64_t cid):id(cid),oracle(cid){

}

TransactionManager::~TransactionManager(){

}

Transaction* TransactionManager::StartTransaction(){
	TransactionId tid = oracle.GetTransactionId();
	Timestamp ts = oracle.GetTimestamp();
	Transaction *t = new Transaction(tid, ts);
	return t;
}

Transaction* TransactionManager::StartTransactionWithDuration(int duration){
	TransactionId tid = oracle.GetTransactionId();
	Timestamp ts = oracle.GetTimestamp();
	Transaction *t = new Transaction(tid,ts,duration);
	return t;
}

bool TransactionManager::ReadData(Transaction* tx, Key key, Value& value){

	if(tx->is_abort){
		return false;
	}

	#ifdef DEBUG
		std::cout<<"HandleRead: Xact_id "<<tx->transaction_id<<endl;
	#endif

	const Value* contains = tx->FindInWriteSet(key);
	if(contains == NULL){
		contains = tx->FindInReadSet(key);
	}
	if(contains != NULL){
		value = *contains;
		tx->AddToPendingSet(key, value, TransactionOperation::READ);
		return true;
	}
	
	TimestampInterval current_interval = tx->committed_interval;
	ServerConnection* server  = service.GetServer(key);
	ReadReply reply;
	server->lock();
	server->client->HandleReadRequest(reply, tx->transaction_id, current_interval, key);
	server->unlock();

	if(reply.state == OperationState::FAIL_NO_VERSION){
		tx->AddToReadSet(key, LockEntry(NULL_VALUE, current_interval));
		tx->AddToPendingSet(key, NULL_VALUE, TransactionOperation::READ);
		value = NULL_VALUE;
		return true;
	}
	if(reply.state == OperationState::R_LOCK_SUCCESS){
		tx->AddToReadSet(key, LockEntry(reply.value, reply.interval));
		tx->AddToPendingSet(key, value, TransactionOperation::READ);
		tx->committed_interval = reply.interval;
		value = reply.value;
		return true;
	}

	AbortTransaction(tx);
	value = NULL_VALUE;
	return false;
}

bool TransactionManager::WriteData(Transaction* tx, Key key, Value value){

	if(tx->is_abort){
		return false;
	}

	#ifdef DEBUG
		std::cout<<"HandleWrite: Xact_id "<<tx->transaction_id<<endl;
	#endif
	const Value* contains = tx->FindInWriteSet(key);
	if(contains != NULL){
		tx->UpdateValue(key, value);
	}

	WriteReply reply;

	TimestampInterval interval = tx->committed_interval;
	auto server = service.GetServer(key);
	server->lock();
	server->client->HandleWriteRequest(reply, tx->transaction_id, 
		tx->committed_interval, key, value);
	server->unlock();
	if (reply.state == OperationState::W_LOCK_SUCCESS) {
		tx->committed_interval = reply.interval; 
		tx->AddToWriteSet(key, LockEntry(value, reply.interval));
		tx->writeSetServers.insert(server);
		tx->AddToPendingSet(key, value, TransactionOperation::WRITE);
		return true;
	}

	//tx->committed_interval = reply.interval;
	AbortTransaction(tx); //other reason, should abort
	return false;
}

bool TransactionManager::CommitTransaction(Transaction* tx){

	if(tx->is_abort){
		#ifdef DEBUG
			std::cout<<"Transaction Xact_id:"<<tx->transaction_id<<" has already be aborted!"<<endl;
		#endif
		return false;
	}

	Timestamp committed_timestamp = tx->committed_interval.start;
	//we can have other policy
	#ifdef DEBUG
		std::cout<<"HandleCommit: Xact_id "<<tx->transaction_id<<",Commit Time:"<<committed_timestamp<<endl;
	#endif

	CommitReply reply;
	bool suss = true;
	for (auto& server: tx->writeSetServers) {
		server->lock();
		server->client->HandleCommit(reply, tx->transaction_id, committed_timestamp);
		suss &= (reply.state == OperationState::COMMIT_OK);
		server->unlock();
	}

	tx->read_set.clear(); 
	tx->write_set.clear();
	tx->writeSetServers.clear();
	tx->pendingOperations.clear();
	return suss;
}

bool TransactionManager::AbortTransaction(Transaction* tx){

	if(tx->is_abort){
		#ifdef DEBUG
			std::cout<<"Transaction Xact_id:"<<tx->transaction_id<<" has already be aborted!"<<endl;
		#endif
		return false;
	}

	#ifdef DEBUG
		std::cout<<"HandleAbort: Xact_id "<<tx->transaction_id<<endl;
	#endif

	AbortReply reply;
	bool suss = true;
	for (auto& server: tx->writeSetServers) {
		server->lock();
		server->client->HandleAbort(reply,tx->transaction_id);
		suss &= (reply.state == OperationState::ABORT_OK);
		server->unlock();
	}
	
	tx->read_set.clear(); 
	tx->write_set.clear();
	tx->writeSetServers.clear();
	return suss;
}

bool TransactionManager::RestartTransaction(Transaction* tx){

	bool suss = false;
	std::vector<std::tuple<Key, Value, TransactionOperation>> operationSet = tx->pendingOperations;
	while(tx->restart_num <= c_restart){
		suss = true;
		tx->restart_num++;
		tx->UpdateInterval(oracle.GetTimestamp());
		tx->pendingOperations.clear();
		for(auto operation:operationSet){
			if(std::get<2>(operation) == TransactionOperation::READ){
				suss &= ReadData(tx, std::get<0>(operation), std::get<1>(operation));
			}
			else if(std::get<2>(operation) == TransactionOperation::WRITE){
				suss &= WriteData(tx, std::get<0>(operation), std::get<1>(operation));
			}
			if(!suss){
				//tx->is_abort = true;
				//tx->pendingOperations.clear();
				break;
			}
		}
		if(suss){
			break;
		}
	}
	tx->is_abort = !suss;
	return suss;

    /*
    std::vector<std::tuple<Key, Value, TransactionOperation>> operationSet = tx->pendingOperations;
    if(tx->restart_num <= c_restart){
        bool suss = true;
        tx->restart_num++;
        std::vector<std::tuple<Key, Value, TransactionOperation>> operationSet = tx->pendingOperations;
        tx->UpdateInterval(oracle.GetTimestamp());
        tx->pendingOperations.clear();
        for(auto operation:operationSet){
            if(std::get<2>(operation) == TransactionOperation::READ){
                suss &= ReadData(tx, std::get<0>(operation), std::get<1>(operation));
            }
            else if(std::get<2>(operation) == TransactionOperation::WRITE){
                suss &= WriteData(tx, std::get<0>(operation), std::get<1>(operation));
            }
            if(!suss){
                break;
            }
        }
        tx->is_abort = !suss;
        return suss;
    }
    else{
        tx->is_abort = true;
        return false;
    }
    */	
}
