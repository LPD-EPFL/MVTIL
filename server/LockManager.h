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
	int top_level;
	struct IL *next[MAX_LEVEL];
	bool is_committed;
}IntervalLock;

class LockManager{

private:
	Key key;
	IntervalLock *head;
	double prob = 0.5;
    int size = 0;
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
	void CommitInterval(TransactionId tid, const Timestamp& old_ts,const Timestamp& commit_ts);
	IntervalLock* CreateReadLock(TimestampInterval read_interval);
	IntervalLock* CreateWriteLock(TimestampInterval write_interval);
	bool RemoveLock(TransactionId tid, const Timestamp& old_ts);
	bool GarbageCollection(Timestamp time);
    int GetSize() {
        return size;
    }

private:
	inline bool intersects(TimestampInterval i1, TimestampInterval i2) {
        if ((i1.finish < i2.start) || (i2.finish < i1.start)) {
            return false;
        }
        return true;
    }

    inline TimestampInterval ComputeIntersection(TimestampInterval i1, TimestampInterval i2) {
        TimestampInterval ts;
        ts.start = MIN_TIMESTAMP;
        ts.finish = MIN_TIMESTAMP;
        if (i1.finish < i2.start) return ts;
        if (i2.finish < i1.start) return ts;
        Timestamp l = std::max(i1.start, i2.start);
        Timestamp r = std::min(i1.finish, i2.finish);
        //if (r < l) return ts;
        ts.start = l;
        ts.finish = r;
        return ts;
    }
};

#endif
