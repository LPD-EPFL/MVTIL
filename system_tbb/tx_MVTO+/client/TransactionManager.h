
#ifndef _TRANSACTION_MANAGER_H_
#define _TRANSACTION_MANAGER_H_

#include "Transaction.h"
#include "LocalOracle.h"
#include "CommunicationService.h"


#define TX_START_RO \
	{ \
	Transaction* t; \
	while (1) { \
		t = transactionManager->StartTransaction();

#define TX_START \
	{ \
	Transaction* t; \
	while (1) { \
		t = transactionManager->StartTransaction();

#define TX_COMMIT \
		if(transactionManager->CommitTransaction(t) == 1){ \
			suss = 1; \
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
		int64_t id; 
		LocalOracle oracle;
		CommunicationService service;
	public:
		TransactionManager(int64_t cid);
		~TransactionManager();
		Transaction* StartTransaction();
		bool CommitTransaction(Transaction* transaction);
		bool AbortTransaction(Transaction* transaction);
		bool RestartTransaction(Transaction* transaction);
		bool ReadData(Transaction* transaction, Key key, Value& value);
		bool WriteData(Transaction* transaction, Key key, Value value);
};

#endif
