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

#define INITIAL_INTERVAL 100
#define INTERVAL_MULTIPLICATION_FACTOR 2
#define INTERVAL_MAX_DURATION 1600
#define RESTART_THRESHOLD 4

#define TX_INIT TransactionManager transactionManager;

#define TX_START_RO \
    Transaction* t; \
    while (1) { \
        t = transactionManager.transactionStart(true);

#define TX_START \
    Transaction* t; \
    while (1) { \
        t = transactionManager.transactionStart(false);

#define TX_END \
        transactionManager.transactionEnd(t); \
        break ; \
    } \
    delete t;

#define TX_READ(key, val) \
    if (transactionManager.read_data(t, key, &val) == 0) { \
        continue; \
    }

#define TX_WRITE(key, val) \
    if (transactionManager.writeData(t, key, val) == 0) { \
        continue; \
    }

#define TX_DECLARE_WRITE(key) \
    if (transactionManager.declareWrite(t, key) == 0) { \
        continue; \
    }

class TransactionManager {
    public:
        TransactionId transactionStart(bool isReadOnly);
        int transactionEnd(Transaction* t);
        int declareWrite(Transaction* t, Key k);
        Value* readData(Transaction* t, Key k);
        int writeData(Transaction* t, Key k, Value v);

    private:
        ConnectionService connectionService;
        TimestampOracle timestampOracle;

        //std::map<TransactionId, Transaction> ongoingTransactions();

        int64_t id; //use this to obtain unique transaction ids

        TransactionId getNewTransactionId();
};

#endif
