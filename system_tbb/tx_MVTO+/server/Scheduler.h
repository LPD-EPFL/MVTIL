#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include "server.h"
#include "DataService.h"
#include "LockSet.h"

#include "tbb/concurrent_hash_map.h"

#define DEFAULT_TIMEOUT 10 //in milliseconds
#define MIN_NUM_RETRIES 5

using namespace tbb;
using namespace ::TxProtocol;

class Scheduler : virtual public DataServiceIf {
  public:
    Scheduler() {
      wait_time = s_timeout;
      read_retry = s_retry;
    }

    ~Scheduler(){ 
    }

    void HandleAbort(AbortReply& _return, const TransactionId tid, const Timestamp ts);
    void HandleCommit(CommitReply& _return, const TransactionId tid, const Timestamp ts);
    void HandleReadRequest(ReadReply& _return, const TransactionId tid, const Timestamp ts, const Key& k);
    void HandleWriteRequest(WriteReply& _return, const TransactionId tid, const Timestamp ts, const Key& k, const Value& v);
    void GetSize(int& version_len);

  private:
    typedef concurrent_hash_map<Key, std::shared_ptr<VersionList>> VersionMap;
    typedef concurrent_hash_map<TransactionId, std::unordered_set<Key>> WriteMap;
    typedef concurrent_hash_map<TransactionId, std::unordered_map<Key, Timestamp>> ReadMap;
    VersionMap versions;//versions for all key
    WriteMap pendingWrite;
    ReadMap pendingRead;
    LockSet lockSet;
    int wait_time;
    int read_retry;
    //std::mutex mutex;

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
