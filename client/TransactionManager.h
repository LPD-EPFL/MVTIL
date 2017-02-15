
#ifndef _TRANSACTION_H_
#define _TRANSACTION_H_

#include "Transaction.h"
#include "LocalOracle.h"
#include "CommunicationServer.h"

class TransactionManager
{

private:
	LocalOracle oracle;
    CommunicationServer servers;
    
public:
	TransactionManager();
	~TransactionManager();
	Transaction* StartTransaction();
	bool CommitTransaction(Transaction* transaction);
	bool AbortTransaction(Transaction* transaction);
	bool ReadData(Transaction* transaction, Key key, Value& value);
	bool WriteData(Transaction* transaction, Key key, Value value);
};

#endif
