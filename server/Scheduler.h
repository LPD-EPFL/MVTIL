
#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include "VersionManager.h"
#include "DataService.h"
#include <chrono>
#include <thread>
#include <unordered_set>

#include "tbb/concurrent_hash_map.h"

class Scheduler : virtual public DataServiceIf {
  public:
	
	Scheduler() {
	  wait_time = s_timeout;
	  retry = s_retry;
	}
	~Scheduler(){}
	
	void HandleAbort(AbortReply& _return, const TransactionId tid);
	void HandleCommit(CommitReply& _return, const TransactionId tid, const Timestamp ts);
	void HandleReadRequest(ReadReply& _return, const TransactionId tid, const TimestampInterval& interval, const Key& k);
	void HandleWriteRequest(WriteReply& _return, const TransactionId tid, const TimestampInterval& interval, const Key& k, const Value& v);
	void GarbageCollection(GCReply& _return, const Timestamp ts);
	void GetSize(int& version_len, int& lock_len);

  private:
	int wait_time;
	int retry;
	VersionManager version_manager;

	//For the concurrent hash table implementation, see https://github.com/efficient/libcuckoo
	//Map
	typedef tbb::concurrent_hash_map<TransactionId, std::unordered_map<Key, std::pair<Value, Timestamp>>> WriteMap;
	typedef tbb::concurrent_hash_map<TransactionId, std::unordered_map<Key, Timestamp>> ReadMap;
	WriteMap pendingWriteSets;
	ReadMap pendingReadSets;
	
	inline bool timeout(std::chrono::time_point<std::chrono::high_resolution_clock> start, std::chrono::time_point<std::chrono::high_resolution_clock> end) {
		auto diff = end - start;
		auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(diff);
		if (ms.count() > wait_time) {
		return true;
		}
		return false;
	}

};

#endif
