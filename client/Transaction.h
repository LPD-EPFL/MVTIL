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

#include <vector>
#include "DataServer_types.h"
#include "common.h"
#include "TransactionManager.h"

class Transaction {
    friend class TransactionManager;
    public:
        Transaction(TransactionId tid, bool RO, TimestampInterval interval) : transactionId(tid), isReadOnly(RO), currentInterval(interval), initialInterval(interval), numRestarts(0) {}
        ~Transaction();
    
        Value alreadyContains(Key k);

    private:
        TransactionId transactionId;
        std::vector<ReadSetEntry> readSet;
        std::vector<WriteSetEntry> writeSet;
        std::vector<HintSetEntry> hintSet;
        std::set<ClientConnection> writeSetServers;

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

            ReadSetEntry(ReadReply r) : key(r.key), value(r.value), interval(r.interval), potential(r.potential) {}
        } ReadSetEntry;

        typedef struct WriteSetEntry {
            Key key;
            Value value;
            TimestampInterval interval;
            TimestampInterval potential;
            ServerAddress address;

            WriteSetEntry(Key k, Value v, ServerAddress s) : key(k), value(v), address(s) {}
        } WriteSetEntry;

        typedef struct HintSetEntry { //FIXME: is it useful for anything to keep this around?
            Key key;
            TimestampInterval interval;
            ServerAddress address;

            HintSetEntry(Key k, TimestampInterval i, ServerAddress a): key(k), interval(i), address(s) {}
        } HintSetEntry;
};

#endif
