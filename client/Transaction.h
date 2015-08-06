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

#include <unordered_set>
#include "DataServer_types.h"
#include "common.h"
#include "TransactionManager.h"

class Transaction {
    friend class TransactionManager;
    public:
        Transaction(TransactionId tid, bool RO, TimestampInterval interval) : transactionId(tid), isReadOnly(RO), currentInterval(interval), initialInterval(interval), numRestarts(0) {}
        ~Transaction();
    
        Value alreadyContains(Key k);

        inline void addToReadSet(ReadSetEntry e) { 
            readSet.insert(e); 
        }
                
        inline void addToWriteSet(WriteSetEntry e) {
            writeSet.insert(e);
        }

        inline void addToHintSet(HintSetEntry e) {
            hintSet.insert(e);
        }

    private:
        TransactionId transactionId;
        std::unordered_set<ReadSetEntry> readSet;
        std::unordered_set<WriteSetEntry> writeSet;
        std::unordered_set<HintSetEntry> hintSet;
        std::unordered_set<ClientConnection> writeSetServers; //TODO use the appropriate type here

        TimestampInterval currentInterval;
        TimestampInterval initialInterval;
        bool isReadOnly;
        uint32_t numRestarts;

        //FIXME do I gain any performance by keeping the connections in these structures? the key to server mapping should be pretty fast
        typedef struct ReadSetEntry {
            Key key;
            Value value;
            TimestampInterval interval;
            TimestampInterval potential;

            ReadSetEntry(Key k, Value v, TimestampInterval i, TimestampInterval p) : key(k), value(v), interval(i), potential(p) {}
        } ReadSetEntry;

        typedef struct WriteSetEntry {
            Key key;
            Value value;
            TimestampInterval interval;
            TimestampInterval potential;
                    
            WriteSetEntry(Key k, Value v, TimestampInterval i, TimestampInterval p) : key(k), value(v), interval(i), potential(p) {}
        } WriteSetEntry;

        typedef struct HintSetEntry { //FIXME: is it useful for anything to keep this around?
            Key key;
            TimestampInterval interval;
            TimestampInterval potential;

            HintSetEntry(Key k, TimestampInterval i): key(k), interval(i) {}
        } HintSetEntry;
};

#endif
