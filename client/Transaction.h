#ifndef _TRANSACTION_H_
#define _TRANSACTION_H_

#include <vector>
#include "DataServer_types.h"
#include "common.h"
#include "TransactionManager.h"

class Transaction {
    friend class TransactionManager;
    public:
        Transaction();
        ~Transaction();
    
        Value alreadyContains(Key k);

    private:
        TransactionId transactionId;
        std::vector<ReadSetEntry> readSet;
        std::vector<WriteSetEntry> writeSet;
        std::vector<HintSetEntry> hintSet;

        TimestampInterval currentInterval;

        typedef struct ReadSetEntry {
            Key k;
            Value v;
            TimestampInterval interval;
            TimestampInterval potential;
        } ReadSetEntry;

        typedef struct WriteSetEntry {
            Key k;
            Value v;
            TimestampInterval interval;
            TimestampInterval potential;
            ServerAddress address;
        } WriteSetEntry;

        typedef struct HintSetEntry {
            Key k;
            TimestampInterval interval;
            ServerAddress address;
        } HintSetEntry;
};

#endif
