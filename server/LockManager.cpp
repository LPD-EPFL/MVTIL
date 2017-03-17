#include "LockManager.h"

LockManager::LockManager(Key k) : key(k){
    head = new IntervalLock;
    head->top_level = MAX_LEVEL;
    head->interval.start = MIN_TIMESTAMP - 1;
    head->interval.finish = MIN_TIMESTAMP - 1;
    head->lock_operation = LockOperation::WRITE;
    IntervalLock *tail = new IntervalLock;
    tail->top_level = 1;
    tail->interval.start = MAX_TIMESTAMP;
    tail->interval.finish = MAX_TIMESTAMP;
    tail->lock_operation = LockOperation::WRITE;
    for(int i=0;i<MAX_LEVEL;i++){
        head->next[i] = tail;
        tail->next[i] = NULL;
    }
}

bool LockManager::LockReadInterval(TimestampInterval &candidate_interval){
    #ifdef DEBUG
        std::cout<<"LockManager: Read interval["<<candidate_interval.start<<","<<candidate_interval.finish<<"]"<<endl;
    #endif
	Timestamp searchTimestamp = candidate_interval.start;
	IntervalLock* node = head;
	for(int level=node->top_level-1; level >=0; level--) {
        while (node->next[level] != NULL && (node->next[level]->interval).start <= searchTimestamp ) {
            node = node->next[level];
        }
    }

    IntervalLock* prev = node;
    IntervalLock* curr = node->next[0];

    //Should I combine the interval????????
    if(candidate_interval.start >= (prev->interval).finish && candidate_interval.finish <= (curr->interval).start){
    	//create new read lock for candidate interval
    	IntervalLock* lock = CreateReadLock(candidate_interval);
    	for(int i=0;i<prev->top_level;i++)
    		prev->next[i] = lock;
    	for(int i=0;i<lock->top_level;i++)
    		lock->next[i] = curr;
    	return true;
    }
    else if(candidate_interval.finish <= (curr->interval).start && !TestConflict(prev->lock_operation, LockOperation::READ)){
    	(prev->interval).finish = candidate_interval.finish;
    	return true;
    }
    else if(candidate_interval.start >= (prev->interval).finish && !TestConflict(curr->lock_operation, LockOperation::READ)){
    	(curr->interval).start = candidate_interval.start;
    	return true;
    }else if(candidate_interval.finish > prev->interval.finish && candidate_interval.finish < curr->interval.start ){
        candidate_interval.start =  prev->interval.finish;
        IntervalLock* lock = CreateReadLock(candidate_interval);
        for(int i=0;i<prev->top_level;i++)
            prev->next[i] = lock;
        for(int i=0;i<lock->top_level;i++)
            lock->next[i] = curr;
        return true;
    }
    return false;
}

bool LockManager::LockWriteInterval(TimestampInterval& candidate_interval){
    #ifdef DEBUG
        std::cout<<"LockManager: Write interval["<<candidate_interval.start<<","<<candidate_interval.finish<<"]"<<endl;
    #endif
	Timestamp searchTimestamp = candidate_interval.start;
	IntervalLock* node = head;
	for(int level=node->top_level-1; level >=0; level--) {
        while (node->next[level] != NULL && (node->next[level]->interval).start <= searchTimestamp ) {
            node = node->next[level];
        }
    }
    IntervalLock* prev = node;
    IntervalLock* curr = node->next[0];
    candidate_interval.start = max(prev->interval.finish,candidate_interval.start);
    candidate_interval.finish = min(curr->interval.start,candidate_interval.finish);
    if(candidate_interval.start >= candidate_interval.finish){
        return false;
    }

    //if(candidate_interval.start >= (prev->interval).finish && candidate_interval.finish <= (curr->interval).start){
    	//create new write lock for candidate interval
    	IntervalLock* lock = CreateWriteLock(candidate_interval);
    	for(int i=0;i<prev->top_level;i++)
    		prev->next[i] = lock;
    	for(int i=0;i<lock->top_level;i++)
    		lock->next[i] = curr;
    	return true;
    //}
    //return false;
}


IntervalLock* LockManager::CreateReadLock(TimestampInterval read_interval){
    #ifdef DEBUG
        std::cout<<"LockManager: Create Read Lock interval["<<read_interval.start<<","<<read_interval.finish<<"]"<<endl;
    #endif
	IntervalLock *lock = new IntervalLock;
	lock->interval = read_interval;
	lock->lock_operation = LockOperation::READ;
	lock->top_level = GetRandomLevel();
	for(int i=0;i<MAX_LEVEL;i++)
		lock->next[i] = NULL;
	return lock;
}

IntervalLock* LockManager::CreateWriteLock(TimestampInterval write_interval){
    #ifdef DEBUG
        std::cout<<"LockManager: Create Write Lock interval["<<write_interval.start<<","<<write_interval.finish<<"]"<<endl;
    #endif
	IntervalLock *lock = new IntervalLock;
	lock->interval = write_interval;
	lock->lock_operation = LockOperation::WRITE;
	lock->top_level = GetRandomLevel();
	for(int i=0;i<MAX_LEVEL;i++)
		lock->next[i] = NULL;
	return lock;
}

bool LockManager::RemoveLock(TimestampInterval write_interval){
    #ifdef DEBUG
        std::cout<<"LockManager: Remove Lock interval["<<write_interval.start<<","<<write_interval.finish<<"]"<<endl;
    #endif
    Timestamp searchTimestamp = write_interval.start;
    IntervalLock* node = head;
    for(int level=node->top_level-1; level >=0; level--) {
        while (node->next[level] != NULL && (node->next[level]->interval).start < searchTimestamp ) {
            node = node->next[level];
        }
    }

    IntervalLock* nextNode = node->next[0];
    if((nextNode->interval).start == write_interval.start && (nextNode->interval).finish == write_interval.finish){
        for(int i=0;i<node->top_level;i++){
            node->next[i] = nextNode->next[0];
        }
        delete nextNode;
        return true;
    }
    return false;
}


void LockManager::GarbageCollection(){

}