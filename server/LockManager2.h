#ifndef _LOCK_MANAGER_H_
#define _LOCK_MANAGER_H_

#include "LockManager.h"

class LockManager2{
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
		LockManager2(Key k);
		bool LockReadInterval(TransactionId tid, TimestampInterval& candidate_interval);
		bool LockWriteInterval(TransactionId tid, TimestampInterval& candidate_interval);
		void CommitInterval(TransactionId tid,const Timestamp& committed_time);
		IntervalLock* CreateReadLock(TimestampInterval read_interval);
		IntervalLock* CreateWriteLock(TimestampInterval write_interval);
		bool RemoveLock(TimestampInterval write_interval);
		bool GarbageCollection(Timestamp time);
};

#endif