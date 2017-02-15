
#include "Transaction.h"

Transaction::~Transaction(){
}

const Value* Transaction::FindInReadSet(Key key){
	if(read_set.count(key) == 0)
		return NULL;
	else
		return &(read_set[key].value);
}


const Value* Transaction::FindInWriteSet(Key key){
	if(write_set.count(key) == 0)
		return NULL;
	else
		return &(write_set[key].value);
}

