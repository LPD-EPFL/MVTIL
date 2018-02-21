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