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

class TransactionManager {
    public:
        TransactionId transactionStart(bool isReadOnly);
        int transactionEnd(TransactionId tid);
        int declareWrite(TransactionId tid, Key k);
        Value* readData(TransactionId tid, Key k);
        int writeData(TransactionId tid, Key k, Value v);

    private:
        RoutingService routingService;
        TimestampOracle timestampOracle;

        std::map<TransactionId, Transaction> ongoingTransactions();

        int64_t id;

        TransactionId getNewTransactionId();
};

#endif
