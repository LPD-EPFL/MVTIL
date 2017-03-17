
#ifndef _TRANSACTION_MANAGER_H_
#define _TRANSACTION_MANAGER_H_

#include "Transaction.h"
#include "LocalOracle.h"
#include "CommunicationService.h"

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
		bool ReadData(Transaction* transaction, Key key, Value& value);
		bool WriteData(Transaction* transaction, Key key, Value value);

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
