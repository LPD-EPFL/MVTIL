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

#ifndef _TRANSACTION_H_
#define _TRANSACTION_H_

#include <unordered_map>
#include <unordered_set>
#include "DataServer_types.h"
#include "common.h"
#include "ServerConnection.h"

// describes an entry in the read, write or hint set
typedef struct SetEntry {
    Value value;
    TimestampInterval interval;
    TimestampInterval potential;

    SetEntry(Value v, TimestampInterval i, TimestampInterval p) :  value(v), interval(i), potential(p) {}
} SetEntry;

// contains the state an ongoing trnsaction
class Transaction {
    friend class TransactionManager;
    public:
    Transaction(TransactionId tid, bool RO, TimestampInterval interval) : transactionId(tid), currentInterval(interval), initialInterval(interval), isReadOnly(RO), numRestarts(0) {}
    ~Transaction();

    Value* alreadyInReadSet(Key key);

    Value* alreadyInWriteSet(Key key);

    inline void addToReadSet(Key k, SetEntry e) { 
        readSet.insert(std::pair<Key, SetEntry>(k,e)); 
    }

    inline void addToWriteSet(Key k, SetEntry e) {
        writeSet.insert(std::pair<Key, SetEntry>(k,e)); 
    }

    inline void addToHintSet(Key k, SetEntry e) {
        hintSet.insert(std::pair<Key, SetEntry>(k,e)); 
    }

    void updateValue(Key k, Value value);

    private:
    TransactionId transactionId;
    std::unordered_map<Key, SetEntry> readSet;
    std::unordered_map<Key, SetEntry> writeSet;
    std::unordered_map<Key, SetEntry> hintSet;
    std::unordered_set<ServerConnection*, ServerConnectionHasher> writeSetServers; 

    TimestampInterval currentInterval;
    TimestampInterval initialInterval;
    bool isReadOnly;
    uint32_t numRestarts;
};

#endif
