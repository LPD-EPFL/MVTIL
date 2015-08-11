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

#ifndef _TRANSACTION_MANAGER_H_
#define _TRANSACTION_MANAGER_H_

#include <cstdint>
#include "common.h"
#include "Transaction.h"
#include "CommunicationService.h"
#include "TimestampOracle.h"

#define RESTART_THRESHOLD 4
#define MULTIPLICATION_FACTOR 10000 //more than the total number of separate client threads (in all clients); could replace with a remote component giving out transaction ids

//some conventience macros for working with transactions
#define TX_START_RO \
    { \
    Transaction* t; \
    while (1) { \
        t = transactionManager->transactionStart(true);

#define TX_START \
    { \
    Transaction* t; \
    while (1) { \
        t = transactionManager->transactionStart(false);

#define TX_END \
        transactionManager->transactionEnd(t); \
        break ; \
    } \
    delete t; \
    }

#define TX_READ(key, val) \
    if (transactionManager->readData(t, key, &val) == 0) { \
        continue; \
    }

#define TX_WRITE(key, val) \
    if (transactionManager->writeData(t, key, val) == 0) { \
        continue; \
    }

#define TX_DECLARE_WRITE(key) \
    if (transactionManager->declareWrite(t, key) == 0) { \
        continue; \
    }

// handles transactions running on the current client
class TransactionManager {
    public:
        TransactionManager(int64_t cid);
        ~TransactionManager();
        Transaction* transactionStart(bool isReadOnly);
        int transactionEnd(Transaction* t);
        int declareWrite(Transaction* t, Key k);
        int readData(Transaction* t, Key k, Value** v);
        int writeData(Transaction* t, Key k, Value v);

    private:
        CommunicationService communicationService;
        TimestampOracle oracle;

        int abortTransaction(Transaction* t);
        int restartTransaction(Transaction* t, Timestamp startBound, Timestamp endBound);

        int64_t id; //use this to obtain unique transaction ids
        int64_t crt;

        TransactionId getNewTransactionId();

        inline bool intersects(TimestampInterval i1, TimestampInterval i2) {
            if ((i1.start > i2.finish) || (i2.start > i1.finish)) {
                return false;
            }
            return true;
        }

        inline TimestampInterval computeIntersection(TimestampInterval i1, TimestampInterval i2) {
            TimestampInterval ts;
            ts.start = MIN_TIMESTAMP;
            ts.finish = MIN_TIMESTAMP;
            if (i1.start > i2.finish) return ts;
            if (i2.start > i1.finish) return ts;
            Timestamp l = std::max(i1.start, i2.start);
            Timestamp r = std::min(i2.finish, i2.finish);
            if (r < l) return ts;
            ts.start = l;
            ts.finish = r;
            return ts;
        }

};

#endif
