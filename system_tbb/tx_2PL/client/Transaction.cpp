#include "Transaction.h"

Transaction::Transaction(TransactionId id):tid(id),restart_num(0),is_abort(false){
}

Transaction::Transaction(TransactionId id,bool isReadOnly):tid(id),restart_num(0),is_read_only(isReadOnly),is_abort(false){
}

Transaction::~Transaction(){
}

const Value* Transaction::FindInReadSet(Key key){
	auto it = read_set.find(key);
    if (it == read_set.end()) {
        return NULL;
    }
    return &(it->second);
}


const Value* Transaction::FindInWriteSet(Key key){
	auto it = write_set.find(key);
    if (it == write_set.end()) {
        return NULL;
    }
    return &(it->second);
}

void Transaction::UpdateValue(Key key, Value update){
	auto it = write_set.find(key);
    if (it == write_set.end()) {
        return;
    }
    it->second = update;
}