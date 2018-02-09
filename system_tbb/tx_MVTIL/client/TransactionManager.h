
#ifndef _TRANSACTION_MANAGER_H_
#define _TRANSACTION_MANAGER_H_

#include "Transaction.h"
#include "LocalOracle.h"
#include "CommunicationService.h"

#define TX_START \
    { \
    Transaction* tx; \
    while (1) { \
        tx = transactionManager->StartTransaction();

#define TX_START_D \
    { \
    Transaction* tx; \
    while (1) { \
        tx = transactionManager->StartTransactionWithDuration(duration);

#define TX_COMMIT \
        if(transactionManager->CommitTransaction(tx) == 1){ \
            suss++; \
        } \
        break ; \
    } \
    delete tx; \
    }

#define TX_READ(key, val) \
    if (transactionManager->ReadData(tx, key, val) == 0) { \
        if(transactionManager->RestartTransaction(tx) == 0) { \
            break; \
        }\
    }

#define TX_WRITE(key, val) \
    if (transactionManager->WriteData(tx, key, val) == 0) { \
        if(transactionManager->RestartTransaction(tx) == 0) { \
            break; \
        }\
    }

#define TX_ABORT \
        transactionManager->AbortTransaction(tx); \
        break ; \
    } \
    delete tx; \
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
		Transaction* StartTransactionWithDuration(int duration);
        bool RestartTransaction(Transaction* tx);
		bool CommitTransaction(Transaction* tx);
		bool AbortTransaction(Transaction* tx);
		bool ReadData(Transaction* tx, Key key, Value& value);
		bool WriteData(Transaction* tx, Key key, Value value);

		inline bool intersects(TimestampInterval i1, TimestampInterval i2) {
            if ((i1.finish < i2.start) || (i2.finish < i1.start)) {
                return false;
            }
            return true;
        }

        inline TimestampInterval ComputeIntersection(TimestampInterval i1, TimestampInterval i2) {
            TimestampInterval ts;
            ts.start = MIN_TIMESTAMP;
            ts.finish = MIN_TIMESTAMP;
            if (i1.finish < i2.start) return ts;
            if (i2.finish < i1.start) return ts;
            Timestamp l = std::max(i1.start, i2.start);
            Timestamp r = std::min(i1.finish, i2.finish);
            //if (r < l) return ts;
            ts.start = l;
            ts.finish = r;
            return ts;
        }

};

#endif