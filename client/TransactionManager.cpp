
#include "TransactionManager.h"

TransactionManager::TransactionManager(int64_t cid):id(cid),oracle(cid){

}

TransactionManager::~TransactionManager(){

}

Transaction* TransactionManager::StartTransaction(){
	TransactionId tid = oracle.GetTransactionId();
	Timestamp ts = oracle.GetTimestamp();
	Transaction *t = new Transaction(tid,ts);
	return t;
}

Transaction* TransactionManager::StartTransactionWithDuration(int duration){
        TransactionId tid = oracle.GetTransactionId();
        Timestamp ts = oracle.GetTimestamp();
        Transaction *t = new Transaction(tid,ts,duration);
        return t;
}

bool TransactionManager::ReadData(Transaction* transaction, Key key, Value& value){

    if(transaction->is_abort)
        return false;

    #ifdef DEBUG
        std::cout<<"HandleRead: Xact_id "<<transaction->transaction_id<<endl;
    #endif

    const Value* contains = transaction->FindInWriteSet(key);
    if(contains == NULL){
        contains = transaction->FindInReadSet(key);
    }
    if(contains != NULL){
        value = *contains;
        return true;
    }
    
    TimestampInterval current_interval = transaction->committed_interval;
    ServerConnection* server  = service.GetServer(key);
    ReadReply reply;
    server->lock();
    server->client->HandleReadRequest(reply, transaction->transaction_id, current_interval, key);
    server->unlock();
    //Should we abort the transaction????
    if(reply.state == OperationState::FAIL_NO_VERSION){
        transaction->AddToReadSet(key, LockEntry(NULL_VALUE, current_interval));
        value = NULL_VALUE;
        return true;
    }
    if(reply.state == OperationState::R_LOCK_SUCCESS){
        transaction->AddToReadSet(key, LockEntry(reply.value, reply.interval));
        transaction->committed_interval = reply.interval;
        value = reply.value;
        return true;
    }

    AbortTransaction(transaction);
    value = NULL_VALUE;
    return false;
}

bool TransactionManager::WriteData(Transaction* transaction, Key key, Value value){

    if(transaction->is_abort)
        return false;

    #ifdef DEBUG
        std::cout<<"HandleWrite: Xact_id "<<transaction->transaction_id<<endl;
    #endif
    const Value* contains = transaction->FindInWriteSet(key);
    if(contains != NULL){
        transaction->UpdateValue(key, value);
        //return true;
    }

    WriteReply reply;
    //while (1) {
        TimestampInterval interval = transaction->committed_interval;
        auto server = service.GetServer(key);
        server->lock();
        server->client->HandleWriteRequest(reply, transaction->transaction_id, 
            transaction->committed_interval, key, value);
        server->unlock();
        if (reply.state == OperationState::W_LOCK_SUCCESS) {

            // #ifdef DEBUG
            //     std::cout<<"WriteData: Key "<<key<<", Value "<<value<<endl;
            // #endif

            //Some problem
            transaction->committed_interval = reply.interval; 
            transaction->AddToWriteSet(key, LockEntry(value, reply.interval));
            transaction->writeSetServers.insert(server);
            return true;
        }

        transaction->committed_interval = reply.interval;
        //?????????value = NULL_VALUE;

        // if (reply.state == OperationState::FAIL_READ_MARK_LARGE) {
        //     if (restartTransaction(transaction, reply.potential.start, MIN_TIMESTAMP) == 1) {
        //         continue;
        //     } else {
        //         return 0;
        //     }
        // }

        // if (reply.state == OperationState::FAIL_INTERSECTION_EMPTY) {
        //     if (restartTransaction(t, reply.potential.start, reply.potential.finish) == 1) {
        //         continue;
        //     } else {
        //         return 0;
        //     }
        // }

        AbortTransaction(transaction); //other reason, should abort

        // #ifdef DEBUG
        //     std::cout<<"Write data conflict "<<value<<" Interval ["<<transaction->committed_interval.start
        //     << " , " << transaction->committed_interval.finish <<"]" <<endl;
        // #endif

        return false;
    //}
}

bool TransactionManager::CommitTransaction(Transaction* transaction){

    if(transaction->is_abort){
        #ifdef DEBUG
            std::cout<<"Transaction has already be aborted!"<<endl;
        #endif
        return false;
    }

    Timestamp committed_timestamp = transaction->committed_interval.start;
    //we can have other policy
    #ifdef DEBUG
        std::cout<<"HandleCommit: Xact_id "<<transaction->transaction_id<<",Commit Time:"<<committed_timestamp<<endl;
    #endif

    CommitReply reply;
    bool suss = true;
    for (auto& server: transaction->writeSetServers) {
        server->lock();
        server->client->HandleCommit(reply, transaction->transaction_id, committed_timestamp);
        suss &= (reply.state == OperationState::COMMIT_OK);
        server->unlock();
    }

    transaction->read_set.clear(); 
    transaction->write_set.clear();
    transaction->writeSetServers.clear();
    return suss;
}

bool TransactionManager::AbortTransaction(Transaction* transaction){

    if(transaction->is_abort){
        #ifdef DEBUG
            std::cout<<"Transaction has already be aborted!"<<endl;
        #endif
        return false;
    }

    #ifdef DEBUG
        std::cout<<"HandleAbort: Xact_id "<<transaction->transaction_id<<endl;
    #endif

    AbortReply reply;
    bool suss = true;
    for (auto& server: transaction->writeSetServers) {
        server->lock();
        server->client->HandleAbort(reply,transaction->transaction_id);
        suss &= (reply.state == OperationState::ABORT_OK);
        server->unlock();
    }
    
    transaction->read_set.clear(); 
    transaction->write_set.clear();
    transaction->writeSetServers.clear();
    transaction->is_abort = true;
    

    // Timespan duration = transaction->initialInterval.finish  transaction->initialInterval.start;
    //TODO when I abort, should I restart with the minimum duration or not?
    // if ((duration * INTERVAL_MULTIPLICATION_FACTOR) < INTERVAL_MAX_DURATION) {
    //     duration *= INTERVAL_MULTIPLICATION_FACTOR;        
    // } else {
    //     duration = INTERVAL_MAX_DURATION;
    // }

    // transaction->transactionId = getNewTransactionId();
    // transaction->initialInterval = oracle.getInterval(transaction->isReadOnly, duration);
    // transaction->currentInterval = transaction->initialInterval; 
    // transaction->numRestarts = 0;
    return suss;
}
