
#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include "common.h"
#include "server.h"
#include "LockSet.h"

#include <unordered_set>
#include <unordered_map>
#include <chrono>
#include <thread>
#include <memory>
#include <iostream>

#include <libcuckoo/cuckoohash_map.hh>
#include <libcuckoo/city_hasher.hh>

using namespace ::TxProtocol;
using namespace std;

typedef enum {READ, WRITE} LockOperation;

class Scheduler : virtual public DataServiceIf {
  public:
    typedef std::pair<LockOperation, std::shared_ptr<std::unordered_set<TransactionId>>> LockEntry;
    typedef std::unordered_map<Key, std::pair<LockOperation,Value>> OperationEntry;
    typedef cuckoohash_map<Key, LockEntry> Locks;
    typedef cuckoohash_map<TransactionId, std::shared_ptr<OperationEntry>> TxMap;
    typedef cuckoohash_map<Key, std::shared_ptr<Value>> DataMap;

    Scheduler() {
      // Your initialization goes here
        wait_time = s_timeout;
        read_retry = s_retry;
    }
    ~Scheduler(){  
    }

    void HandleAbort(AbortReply& _return, const TransactionId tid);
    void HandleCommit(CommitReply& _return, const TransactionId tid);
    void HandleReadRequest(ReadReply& _return, const TransactionId tid, const Key& k, const bool isReadOnly);
    void HandleWriteRequest(WriteReply& _return, const TransactionId tid, const Key& k, const Value& v);
    void GetSize(int& lock_len);

  private:
    Locks data_locks;
    TxMap pending_operations;
    DataMap persist_versions;
    LockSet lockSet;
    int wait_time;
    int read_retry;

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
