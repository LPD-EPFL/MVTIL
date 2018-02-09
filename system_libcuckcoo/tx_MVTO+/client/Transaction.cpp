
#include "Transaction.h"

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