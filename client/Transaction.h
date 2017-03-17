
#ifndef _TRANSACTION_H_
#define _TRANSACTION_H_

#include "ServerConnection.h"
#include <unordered_map>
#include <unordered_set>

#define TRANSACTION_MAX_DURATION 16

class Transaction{

	friend class TransactionManager;
	private:
		TransactionId transaction_id;
		Timestamp start_time;
		TimestampInterval committed_interval;
		std::unordered_map<Key,LockEntry> read_set;
		std::unordered_map<Key,LockEntry> write_set;
		std::unordered_set<ServerConnection*, ServerConnectionHasher> writeSetServers; 

	public:
		Transaction(TransactionId tid, Timestamp start, Timestamp duration):transaction_id(tid),start_time(start){
			committed_interval.start = start;
			committed_interval.finish = start + duration;
		}
		Transaction(TransactionId tid, Timestamp start):transaction_id(tid),start_time(start){
			committed_interval.start = start;
			committed_interval.finish = start + TRANSACTION_MAX_DURATION;
		}
		~Transaction();
		const Value* FindInReadSet(Key key);
		const Value* FindInWriteSet(Key key);
		inline void AddToReadSet(Key k, LockEntry e) { 
	        read_set.insert(std::pair<Key, LockEntry>(k,e)); 
	    }

	    inline void AddToWriteSet(Key k, LockEntry e) {
	        write_set.insert(std::pair<Key, LockEntry>(k,e)); 
	    } 

	    void UpdateValue(Key key, Value update);
};


#endif
