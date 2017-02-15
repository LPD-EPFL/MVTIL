
#ifndef _TRANSACTION_H
#define _TRANSACTION_H

#include <unordered_map>
#include "common.h"

class Transaction{

	private:
		TransactionId transaction_id;
		Timestamp start_time;
		TimestampInterval committed_interval;
		std::unordered_map<Key,LockEntry> read_set;
		std::unordered_map<Key,LockEntry> write_set;

	public:
		Transaction(TransactionId tid, Timestamp time):transaction_id(tid),start_time(time){}
		~Transaction();
		const Value* FindInReadSet(Key key);
		const Value* FindInWriteSet(Key key);

};


#endif
