#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include "server.h"
#include "DataService.h"
#include "LockSet.h"

#include <libcuckoo/cuckoohash_map.hh>
#include <libcuckoo/city_hasher.hh>

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
    typedef std::unordered_set<Key> WriteEntry;
    typedef std::unordered_map<Key, Timestamp> ReadEntry;
    typedef cuckoohash_map<Key, std::shared_ptr<VersionList>> VersionMap;
    typedef cuckoohash_map<TransactionId, std::shared_ptr<WriteEntry>> WriteMap;
    typedef cuckoohash_map<TransactionId, std::shared_ptr<ReadEntry>> ReadMap;
    //versions for all key
    VersionMap versions;
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
