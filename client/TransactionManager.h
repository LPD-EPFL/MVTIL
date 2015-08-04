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
