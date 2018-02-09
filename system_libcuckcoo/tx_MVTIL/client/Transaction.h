
#ifndef _TRANSACTION_H_
#define _TRANSACTION_H_

#include "ServerConnection.h"
#include <tuple>
#include <vector>
#include <unordered_map>
#include <unordered_set>

typedef enum {READ, WRITE} TransactionOperation;

class Transaction{

	friend class TransactionManager;
	private:
		TransactionId transaction_id;
		Timestamp start_time;
		TimestampInterval committed_interval;
		std::unordered_map<Key,LockEntry> read_set;
		std::unordered_map<Key,LockEntry> write_set;
		std::unordered_set<ServerConnection*, ServerConnectionHasher> writeSetServers;
		std::vector<std::tuple<Key, Value, TransactionOperation>> pendingOperations;
		int restart_num;
		bool is_abort;

	public:
		Transaction(TransactionId tid, Timestamp start, Timestamp duration):transaction_id(tid),start_time(start){
			committed_interval.start = start;
			committed_interval.finish = start + (duration << LOW_BITE);
			is_abort = false;
			restart_num = 0;
		}
		Transaction(TransactionId tid, Timestamp start):transaction_id(tid),start_time(start){
			committed_interval.start = start;
			committed_interval.finish = start + (c_interval_duration << LOW_BITE);
			is_abort = false;
			restart_num = 0;
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

	    inline void AddToPendingSet(Key k, Value v, TransactionOperation op){
	    	pendingOperations.push_back(std::make_tuple(k, v, op));
	    }

	    void UpdateInterval(Timestamp start){
	    	committed_interval.start = start;
	    	committed_interval.finish = start + (c_interval_duration << LOW_BITE);
	    }

	    void UpdateValue(Key key, Value update);

};


#endif
