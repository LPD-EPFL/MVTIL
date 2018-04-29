/*   
 *   File: TransactionManager.cpp
 *   Author: Junxiong Wang <junxiong.wang@epfl.ch>
 *   Description: 
 *   TransactionManager.cpp is part of MVTLSYS
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

bool TransactionManager::ReadData(Transaction* tx, Key key, Value& value){

    #ifdef DEBUG
        std::cout<<"HandleRead: Xact_id "<<tx->tid<<endl;
    #endif

    if(tx->is_abort){
        return false;
    }

    const Value* contains = tx->FindInWriteSet(key);
    if(contains == NULL){
        contains = tx->FindInReadSet(key);
    }
    if(contains != NULL){
        value = *contains;
        return true;
    }
    
    ServerConnection* server  = service.GetServer(key);
    ReadReply reply;
    server->lock();
    server->client->HandleReadRequest(reply, tx->tid, tx->ts, key);
    server->unlock();
    //Should we abort the transaction????
    if(reply.state == OperationState::FAIL_NO_VERSION){
        tx->AddToReadSet(key, NULL_VALUE);
        tx->AddToPendingSet(key, NULL_VALUE, TransactionOperation::READ);
        value = NULL_VALUE;
        return true;
    }
    if(reply.state == OperationState::R_SUCCESS){
        tx->AddToReadSet(key, reply.value);
        tx->AddToPendingSet(key, NULL_VALUE, TransactionOperation::READ);
        value = reply.value;
        return true;
    }

    AbortTransaction(tx);
    value = NULL_VALUE;
    return false;
}

bool TransactionManager::WriteData(Transaction* tx, Key key, Value value){

    #ifdef DEBUG
        std::cout<<"HandleWrite: Xact_id "<<tx->tid<<endl;
    #endif

    if(tx->is_abort){
        return false;
    }

    const Value* contains = tx->FindInWriteSet(key);
    if(contains != NULL){
        tx->UpdateValue(key, value);
    }

    WriteReply reply;
    auto server = service.GetServer(key);
    server->lock();
    server->client->HandleWriteRequest(reply, tx->tid, 
        tx->ts, key, value);
    server->unlock();
    if (reply.state == OperationState::W_SUCCESS) {
        tx->AddToWriteSet(key, value);
        tx->writeSetServers.insert(server);
        tx->AddToPendingSet(key, value, TransactionOperation::WRITE);
        return true;
    }
    AbortTransaction(tx);
    return false;
}

bool TransactionManager::CommitTransaction(Transaction* tx){

    if(tx->is_abort){
        return false;
    }
    CommitReply reply;
    for (auto& server: tx->writeSetServers) {
        server->lock();
        server->client->HandleCommit(reply, tx->tid, tx->ts);
        server->unlock();
    }

    tx->read_set.clear(); 
    tx->write_set.clear();
    tx->writeSetServers.clear();
    return true;
}

bool TransactionManager::AbortTransaction(Transaction* tx){

    if(tx->is_abort){
        return false;
    }

    AbortReply reply;
    for (auto& server: tx->writeSetServers) {
        server->lock();
        server->client->HandleAbort(reply, tx->tid, tx->ts);
        server->unlock();
    }
    
    tx->read_set.clear(); 
    tx->write_set.clear();
    tx->writeSetServers.clear();
    return true;
}

bool TransactionManager::RestartTransaction(Transaction* tx){
     bool suss = false;
     std::vector<std::tuple<Key, Value, TransactionOperation>> operationSet = tx->pendingOperations;
     while(tx->restart_num <= c_restart && !suss){
        suss = true;
        tx->restart_num++;
        tx->UpdateTimestamp(oracle.GetTimestamp());
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
     }
     tx->is_abort = !suss;
     return suss;
}

