#ifndef _TRANSACTION_MANAGER_H_
#define _TRANSACTION_MANAGER_H_

#include "client.h"
#include "Transaction.h"
#include "CommunicationService.h"

#define MAX_FACTOR 10000

#define TX_START_RO \
    { \
    Transaction* t; \
    while (1) { \
        t = transactionManager->StartTransaction(true);

#define TX_START \
    { \
    Transaction* t; \
    while (1) { \
        t = transactionManager->StartTransaction(false);

#define TX_COMMIT \
        if(transactionManager->CommitTransaction(t) == 1){ \
            suss++; \
        } \
        break ; \
    } \
    delete t; \
    }

#define TX_READ(key, val) \
    if (transactionManager->ReadData(t, key, val) == 0) { \
        if(transactionManager->RestartTransaction(t) == 0) { \
            break; \
        }\
    }

#define TX_WRITE(key, val) \
    if (transactionManager->WriteData(t, key, val) == 0) { \
        if(transactionManager->RestartTransaction(t) == 0) { \
            break; \
        }\
    }

#define TX_ABORT \
        transactionManager->AbortTransaction(t); \
        break ; \
    } \
    delete t; \
    }

class TransactionManager
{

private:
	int64_t client_id;
	int64_t crt;
	CommunicationService service;

public:
	TransactionManager(int64_t cid);
	~TransactionManager();
	Transaction* StartTransaction();
    Transaction* StartTransaction(bool isReadOnly);
    bool RestartTransaction(Transaction *tx);
	bool CommitTransaction(Transaction *tx);
	bool AbortTransaction(Transaction *tx);
	bool ReadData(Transaction *tx, Key key, Value &value);
	bool WriteData(Transaction *tx, Key key, Value value);
};

#endif
