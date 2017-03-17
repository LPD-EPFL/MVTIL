#ifndef _INTERVAL_LOCK_H_
#define _INTERVAL_LOCK_H_

#include "server.h"

#define MAX_LEVEL 4

typedef struct IL{
	TimestampInterval interval;
	LockOperation lock_operation;
	Value value;
	int top_level;
	struct IL *next[MAX_LEVEL];
}IntervalLock;

class LockManager{

private:
	Key key;
	IntervalLock *head;
	double prob = 0.5;
	
	int GetRandomLevel() {
        int level = 1;        
        while ( rand_range(101) < 100*prob && level < MAX_LEVEL )
            level++;
        return level;
    }

public:
	LockManager(Key k);
	bool LockReadInterval(TimestampInterval& candidate_interval);
	bool LockWriteInterval(TimestampInterval& candidate_interval);
	IntervalLock* CreateReadLock(TimestampInterval read_interval);
	IntervalLock* CreateWriteLock(TimestampInterval write_interval);
	bool RemoveLock(TimestampInterval write_interval);
	void GarbageCollection();
};

#endif