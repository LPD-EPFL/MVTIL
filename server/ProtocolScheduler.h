/* 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _PROTOCOL_SCHEDULER_H_
#define _PROTOCOL_SCHEDULER_H_

#include <unordered_set>
#include <queue>
#include <chrono>
#include <mutex>
#include <thread>
#include <libcuckoo/cuckoohash_map.hh>
#include <libcuckoo/city_hasher.hh>
#include <iostream>
#include "common.h"
#include "VersionManager.h"
//#include "Timer.h"
#include "DataServer.h"
#include "DataServer_types.h"

using namespace  ::TxProtocol;

#define MIN_NUM_RETRIES 3 //the minimum number of times a read operation must retry before giving up

#ifdef INITIAL_TESTING
typedef std::string DsKey;
#endif

#define DEFAULT_TIMEOUT 100 //in milliseconds

// Handles incoming messages from clients or other servers and schedules events on the server accordingly
class ProtocolScheduler : virtual public DataServerIf {
    private:
        //write set entry: describes a single pending write
        typedef struct WSEntry{
            Version *version;
            Key key;
            Value value;

            WSEntry(Version* ver, Key k, Value val) : version(ver), key(k), value(val) {}
            WSEntry() : version(), key(), value() {}
        } WSEntry;
       
        //the write set of a single pending transaction; access to it is protected by a mutex
        typedef struct WriteSet {
            std::unordered_set<WSEntry*> pendingWrites;
            std::recursive_mutex mutex;

            inline void lock() {
                mutex.lock();
            }

            inline void unlock() {
                mutex.unlock();
            }
        } WriteSet;

    public:
        ProtocolScheduler();

        ~ProtocolScheduler();

        void handleReadRequest(ReadReply& _return, const TransactionId tid, const TimestampInterval& interval, const Key& k, const bool isReadOnly);

        void handleWriteRequest(WriteReply& _return, const TransactionId tid, const TimestampInterval& interval, const Key& k, const Value& v);

        void handleHintRequest(HintReply& _return, const TransactionId tid, const TimestampInterval& interval, const Key& k);

        void handleCommit(CommitReply& _return, const TransactionId tid, const Timestamp ts);


        void handleAbort(AbortReply& _return, const TransactionId tid);

        // handles a generic operation (e.g., a RMW)
        void handleOperation(ServerGenericReply& _return, const ClientGenericRequest& cr);

        // if a transaction needs to be restarted (and not completely aborted), 
        // the coordinator might pick a different interval for operations it has done so far; 
        // this method tries to adjust the read locks on a version accordingly 
        // note that the version which is locked cannot change
        void handleExpandRead(ExpandReadReply& _return, const TransactionId tid, const Timestamp versionTimestamp, const TimestampInterval& newInterval, const Key& k);


        // if a transaction needs to be restarted (and not completely aborted), 
        // the coordinator might pick a different interval for operations it has done so far; 
        // this method tries to adjust the corresponding pending vesion accordingly 
        void handleExpandWrite(ExpandWriteReply& _return, const TransactionId tid, const Timestamp versionTimestamp, const TimestampInterval& newInterval, const Key& k);
#ifndef INITIAL_TESTING
        // this method gets called when a garbage collection round should be run
        void handleNewEpoch(Timestamp barrier);
#endif

    private:
        VersionManager versionManager;
#ifndef INITIAL_TESTING
        DataStore* dataStore;
#endif
        
        DsKey* toDsKey(Key k, Timestamp ts);

        void abortTransaction(TransactionId tid);

        void garbageCollect(Timestamp barrier);

        // pending write sets of all the ongoing transactions; using a concurrent hash table,
        // as multiple threads may be inserting/removing/searching transactions at the same time;
        // for the concurrent hash table implementation, see https://github.com/efficient/libcuckoo
        cuckoohash_map<TransactionId, WriteSet*, CityHasher<TransactionId>> pendingWriteSets;

        inline bool timeout(std::chrono::time_point<std::chrono::high_resolution_clock> start, std::chrono::time_point<std::chrono::high_resolution_clock> end) {
           auto diff = end - start;
           auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(diff);
           if (ms.count() > DEFAULT_TIMEOUT) {
            return true;
           }
           return false;
        }

        //std::map<TransactionId, WriteSet*> pendingWriteSets; //TODO needs to be concurrent!

};
#endif
