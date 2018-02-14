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

#ifndef _TRANSACTION_H_
#define _TRANSACTION_H_

#include "ServerConnection.h"
#include <unordered_map>
#include <unordered_set>

typedef enum {READ, WRITE} TransactionOperation;

class Transaction{

	friend class TransactionManager;
	private:
		TransactionId tid;
		Timestamp ts;
		std::unordered_map<Key,Value> read_set;
		std::unordered_map<Key,Value> write_set;
		std::unordered_set<ServerConnection*, ServerConnectionHasher> writeSetServers;
		std::vector<std::tuple<Key, Value, TransactionOperation>> pendingOperations;
		int restart_num;
		bool is_abort;

	public:
		Transaction(TransactionId tranx_id, Timestamp start):tid(tranx_id),ts(start),is_abort(false){}
		~Transaction();
		const Value* FindInReadSet(Key key);
		const Value* FindInWriteSet(Key key);
		inline void AddToReadSet(Key k, Value e) { 
	        read_set.insert(std::pair<Key, Value>(k,e)); 
	    }

	    inline void AddToWriteSet(Key k, Value e) {
	        write_set.insert(std::pair<Key, Value>(k,e)); 
	    }
		
	    inline void AddToPendingSet(Key k, Value v, TransactionOperation op){
		pendingOperations.push_back(std::make_tuple(k, v, op));
	    }
	
	    void UpdateTimestamp(Timestamp new_ts){
	    	ts = new_ts;
	    }

	    void UpdateValue(Key key, Value update);
};


#endif
