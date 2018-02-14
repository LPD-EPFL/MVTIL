/*   
 *   File: Transaction.cpp
 *   Author: Junxiong Wang <junxiong.wang@epfl.ch>
 *   Description: 
 *   Transaction.cpp is part of MVTLSYS
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
 
#ifndef _TRANSACTION_H_
#define _TRANSACTION_H_

#include "common.h"
#include "ServerConnection.h"
#include <unordered_map>
#include <unordered_set>

using namespace  ::TxProtocol;

typedef enum {READ, WRITE} TransactionOperation;

class Transaction
{
	friend class TransactionManager;
	private:
		TransactionId tid;
		std::unordered_map<Key,Value> read_set;
		std::unordered_map<Key,Value> write_set;
		std::unordered_set<ServerConnection*, ServerConnectionHasher> lockSetServers;
		std::vector<std::tuple<Key, Value, TransactionOperation>> pendingOperations;
		int restart_num;
		bool is_read_only;
		bool is_abort;

	public:
		Transaction(TransactionId id);
		Transaction(TransactionId id,bool isReadOnly);
		~Transaction();
		inline void AddToReadSet(Key key, Value value){
			read_set.insert(std::pair<Key,Value>(key,value));
		}
		inline void AddToWriteSet(Key key, Value value){
			write_set.insert(std::pair<Key,Value>(key,value));
		}
		inline void AddToPendingSet(Key k, Value v, TransactionOperation op){
			pendingOperations.push_back(std::make_tuple(k, v, op));
		}
		const Value* FindInReadSet(Key key);
		const Value* FindInWriteSet(Key key);
		void UpdateValue(Key key, Value value);
};

#endif
