#ifndef _TRANSACTION_MANAGER_H_
#define _TRANSACTION_MANAGER_H_

#include <common.h>
#include <cstdint>
#include "Transaction.h"
#include "CommunicationService.h"
#include "TimestampOracle.h"

class TransactionManager {
    public:
        void transactionStart(bool isReadOnly);
        void transactionEnd();
        void declareWrite(Key k);
        Value readData(Key k);
        void writeData(Key k, Value v);

    private:
        RoutingService routingService;
        TimestampOracle timestampOracle;

        std::vector<Transaction> ongoingTransactions();

        int64_t id;

        TransactionId getNewTransactionId();
};

#endif
