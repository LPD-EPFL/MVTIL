#ifndef _INTERVAL_LOCK_H_
#define _INTERVAL_LOCK_H_

#include "server.h"
#include "CapabilityOperation.h"
#include <mutex>

//#define MAX_LEVEL 4

typedef struct IL{
	TransactionId transaction_id; 
	TimestampInterval interval;
	LockOperation lock_operation;
	//Value value;
	int top_level;
	struct IL *next[MAX_LEVEL];
	bool is_committed;
}IntervalLock;

class LockManager{

private:
	Key key;
	IntervalLock *head;
	double prob = 0.5;
	//std::mutex mutex;
	
	int GetRandomLevel() {
        int level = 1;        
        while ( rand_range(101) < 100*prob && level < MAX_LEVEL )
            level++;
        return level;
    }

public:
	LockManager(Key k);
	bool LockReadInterval(TransactionId tid, TimestampInterval& candidate_interval);
	bool LockWriteInterval(TransactionId tid, TimestampInterval& candidate_interval);
	void CommitInterval(TransactionId tid, const Timestamp& committed_time);
	IntervalLock* CreateReadLock(TimestampInterval read_interval);
	IntervalLock* CreateWriteLock(TimestampInterval write_interval);
	bool RemoveLock(TransactionId tid, TimestampInterval interval);
	bool GarbageCollection(Timestamp time);
};

#endif
