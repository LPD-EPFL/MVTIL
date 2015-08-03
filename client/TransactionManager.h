#ifndef _TRANSACTION_MANAGER_H_
#define _TRANSACTION_MANAGER_H_

#include <common.h>
#include <cstdint>
#include "Transaction.h"
#include "CommunicationService.h"
#include "TimestampOracle.h"

class TransactionManager {
    public:
        void transactionStart();
        void transactionEnd();
        Value readData(Key k);
        void writeData(Key k, Value v);
    private:
        RoutingService routingService;
        TimestampOracle timestampOracle;

        int64_t id;

        TransactionId getNewTransactionId();
};

#endif
