#include "LockManager.h"

LockManager::LockManager(Key k) : key(k){
	head = new IntervalLock;
	head->top_level = MAX_LEVEL;
	head->interval.start = MIN_TIMESTAMP;
	head->interval.finish = MIN_TIMESTAMP;
	head->lock_operation = LockOperation::WRITE;
	head->transaction_id = 0;
	head->is_committed = true;
	IntervalLock *tail = new IntervalLock;
	tail->top_level = 1;
	tail->interval.start = MAX_TIMESTAMP;
	tail->interval.finish = MAX_TIMESTAMP;
	tail->lock_operation = LockOperation::WRITE;
	tail->transaction_id = 0;
	tail->is_committed = true;
	for(int i=0;i<MAX_LEVEL;i++){
		head->next[i] = tail;
		tail->next[i] = NULL;
	}
}

bool LockManager::LockReadInterval(TransactionId tid, TimestampInterval& candidate_interval){
	#ifdef DEBUG
		std::cout<<"LockManager: Read interval["<<candidate_interval.lock_start<<","<<candidate_interval.finish<<"]"<<endl;
	#endif
	Timestamp searchTimestamp = candidate_interval.lock_start;
	IntervalLock* node = head;
	for(int level=node->top_level-1; level >=0; level--) {
		while (node->next[level] != NULL && (node->next[level]->interval).start <= searchTimestamp ) {
			//if((node->next[level]->interval.finish >= candidate_interval.finish) && TestConflict(node->next[level]->lock_operation,LockOperation::READ)){
			if(!node->next[level]->is_committed && (node->next[level]->interval.finish >= searchTimestamp) && TestConflict(node->next[level]->lock_operation,LockOperation::READ)){
			   #ifdef DEBUG
				  std::cout<<"Read conflict happens!"<<endl;
			   #endif
			   return false; 
			}
			// else if(TestConflict(node->next[level]->lock_operation,LockOperation::READ)){
			//     candidate_interval.finish = min(candidate_interval.finish,);
			// }
			node = node->next[level];
		}
	}

	IntervalLock* prev = node;
	IntervalLock* curr = node->next[0];

	//One policy
	candidate_interval.finish = min(candidate_interval.finish, curr->interval.start - 1);

	if(candidate_interval.start > candidate_interval.finish){
		return false;
	}

	TimestampInterval lock_interval;
	lock_interval.start = candidate_interval.lock_start;
	lock_interval.finish = candidate_interval.finish;

	//mutex.lock();
	IntervalLock* lock = CreateReadLock(lock_interval);
	lock->transaction_id = tid;
	for(int i=0;i<prev->top_level;i++)
		prev->next[i] = lock;
	for(int i=0;i<lock->top_level;i++)
		lock->next[i] = curr;
	//mutex.unlock();
	return true;

	//Two policy largest interval first

	//Should I combine the interval????????
	// if(candidate_interval.start >= (prev->interval).finish && candidate_interval.finish <= (curr->interval).start){
	// 	//create new read lock for candidate interval
	// 	IntervalLock* lock = CreateReadLock(candidate_interval);
	//     lock->transaction_id = tid;
	// 	for(int i=0;i<prev->top_level;i++)
	// 		prev->next[i] = lock;
	// 	for(int i=0;i<lock->top_level;i++)
	// 		lock->next[i] = curr;
	// 	return true;
	// }
	// else if(candidate_interval.finish <= (curr->interval).start && !TestConflict(prev->lock_operation, LockOperation::READ)){
	// 	(prev->interval).finish = candidate_interval.finish;
	// 	return true;
	// }
	// else if(candidate_interval.start >= (prev->interval).finish && !TestConflict(curr->lock_operation, LockOperation::READ)){
	// 	(curr->interval).start = candidate_interval.start;
	// 	return true;
	//}

	// else if(candidate_interval.finish > prev->interval.finish && candidate_interval.finish < curr->interval.start ){
	//     candidate_interval.start =  prev->interval.finish;
	//     IntervalLock* lock = CreateReadLock(candidate_interval);
	//     for(int i=0;i<prev->top_level;i++)
	//         prev->next[i] = lock;
	//     for(int i=0;i<lock->top_level;i++)
	//         lock->next[i] = curr;
	//     return true;
	// }
	// return false;
}

bool LockManager::LockWriteInterval(TransactionId tid, TimestampInterval& candidate_interval){
	#ifdef DEBUG
		std::cout<<"LockManager: Write interval["<<candidate_interval.start<<","<<candidate_interval.finish<<"]"<<endl;
	#endif
	Timestamp searchTimestamp = candidate_interval.start;
	IntervalLock* node = head;
	int level = 0;
	//for(int level=node->top_level-1; level >=0; level--) {
		while (node->next[level] != NULL && (node->next[level]->interval).start <= searchTimestamp ) {
			// #ifdef DEBUG
			//     std::cout<<(node->next[level]->interval).start<<" "<<(node->next[level]->interval).finish<<endl;
			// #endif
			if(node->next[level]->transaction_id != tid && (node->next[level]->interval.finish > searchTimestamp) && TestConflict(node->next[level]->lock_operation,LockOperation::WRITE)){
			   searchTimestamp = (node->next[level])->interval.finish + 1;
			}
			if(searchTimestamp > candidate_interval.finish){
				#ifdef DEBUG
					std::cout<<"Write conflict happens!"<<endl;
				#endif
				return false;
			}
			node = node->next[level];
		}
	//}
	IntervalLock* prev = node;
	IntervalLock* curr = node->next[0];
	candidate_interval.start = searchTimestamp;
	candidate_interval.finish = min(curr->interval.start - 1,candidate_interval.finish);
	if(candidate_interval.start > candidate_interval.finish){
		#ifdef DEBUG
			std::cout<<"Write conflict happens!"<<endl;
		#endif
		return false;
	}

	//candidate_interval.start = max(prev->interval.finish,candidate_interval.start);
	//candidate_interval.finish = min(curr->interval.start,candidate_interval.finish);
	//if(candidate_interval.start >= candidate_interval.finish){
		//return false;
	//}

	//if(candidate_interval.start >= (prev->interval).finish && candidate_interval.finish <= (curr->interval).start){
	//create new write lock for candidate interval

	IntervalLock* lock = CreateWriteLock(candidate_interval);
	lock->transaction_id = tid;
	for(int i=0;i<prev->top_level;i++)
		prev->next[i] = lock;
	for(int i=0;i<lock->top_level;i++)
		lock->next[i] = curr;
	return true;
}

void LockManager::CommitInterval(TransactionId tid, const Timestamp& committed_time){
	IntervalLock* node = head;
	int level = 0;
	//for(int level=node->top_level-1; level >=0; level--) {
		while (node->next[level] != NULL && (node->next[level]->interval).start <= committed_time ) {
			if(node->next[level]->transaction_id == tid){
				node->next[level]->is_committed = true;
				if(node->next[level]->lock_operation == LockOperation::WRITE)
				{
					#ifdef DEBUG
						std::cout<<"LockManager: Modify Write Lock interval["<<committed_time<<","<<committed_time<<"]"<<std::endl;
					#endif
					node->next[level]->interval.start = committed_time;
					node->next[level]->interval.finish = committed_time;
				}
				else
				{
					#ifdef DEBUG
						std::cout<<"LockManager: Modify Read Lock interval to ["<<committed_time<<"]"<<std::endl;
					#endif
					node->next[level]->interval.finish = committed_time;
				}
			}
			node = node->next[level];
		}
	//}


	#ifdef DEBUG
		node = head;
		while (node->next[0] != NULL) {
			std::cout<<"["<<node->interval.start<<","<<node->interval.finish<<"]"<<" ";          
			node = node->next[0];
		}
		std::cout<<std::endl;
	#endif

}


IntervalLock* LockManager::CreateReadLock(TimestampInterval read_interval){
	#ifdef DEBUG
		std::cout<<"LockManager: Create Read Lock interval["<<read_interval.start<<","<<read_interval.finish<<"]"<<std::endl;
	#endif
	IntervalLock *lock = new IntervalLock;
	lock->interval = read_interval;
	lock->lock_operation = LockOperation::READ;
	lock->is_committed = false;
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
	lock->is_committed = false;
	lock->top_level = GetRandomLevel();
	for(int i=0;i<MAX_LEVEL;i++)
		lock->next[i] = NULL;
	return lock;
}

bool LockManager::RemoveLock(TransactionId tid, TimestampInterval interval){
	#ifdef DEBUG
		std::cout<<"LockManager: Remove Lock interval["<<interval.start<<","<<interval.finish<<"]"<<endl;
	#endif

	/*
	Timestamp searchTimestamp = interval.start;
	IntervalLock* node = head;
	for(int level=node->top_level-1; level >=0; level--) {
		while (node->next[level] != NULL && (node->next[level]->interval).start < searchTimestamp ) {
			node = node->next[level];
		}
	}

	IntervalLock* nextNode = node->next[0];
	while((nextNode->interval).start == interval.start)
	{
		if((nextNode->interval).start == interval.start && (nextNode->interval).finish == interval.finish){
			for(int i=0;i<node->top_level;i++){
				node->next[i] = nextNode->next[0];
			}
			delete nextNode;
			return true;
		}
		nextNode = nextNode->next[0];
	}
	return false;
	*/

	Timestamp searchTimestamp = interval.start;
	IntervalLock* node = head;
	int level = 0;
	//for(int level=node->top_level-1; level >=0; level--) {
		while (node->next[level] != NULL && (node->next[level]->interval).start <= searchTimestamp ) {
			if(node->next[level]->transaction_id == tid){
				IntervalLock* nextNode = node->next[0];
				for(int i=0;i<node->top_level;i++){
					node->next[i] = nextNode->next[0];
				}
				delete nextNode;				
			}
			else{
				node = node->next[level];
			}
		}
	//}
	return true;
}

bool LockManager::GarbageCollection(Timestamp ts){
	IntervalLock* node = head->next[0];
	while (node->next[0] != NULL && (node->next[0]->interval).start <= ts) {
		IntervalLock* cur = node;
		node = node->next[0];
		delete cur;
	}
	for(int i=0;i<MAX_LEVEL;i++){
		head->next[i] = node;
	}
	return true;
}
