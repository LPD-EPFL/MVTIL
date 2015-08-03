#ifndef _TRANSACTION_H_
#define _TRANSACTION_H_

#include <vector>
#include "DataServer_types.h"
#include "common.h"
#include "TransactionManager.h"

class Transaction {
    friend class TransactionManager;
    public:
        Transaction(TransactionId tid, bool RO) : transactionId(tid), isReadOnly(RO) {}
        ~Transaction();
    
        Value alreadyContains(Key k);

    private:
        TransactionId transactionId;
        std::vector<ReadSetEntry> readSet;
        std::vector<WriteSetEntry> writeSet;
        std::vector<HintSetEntry> hintSet;

        TimestampInterval currentInterval;
        bool isReadOnly;

        typedef struct ReadSetEntry {
            Key key;
            Value value;
            TimestampInterval interval;
            TimestampInterval potential;

            ReadSetEntry(ReadReply r) : key(r.key), value(r.value), interval(r.interval), potential(r.potential) {}
        } ReadSetEntry;

        typedef struct WriteSetEntry {
            Key key;
            Value value;
            TimestampInterval interval;
            TimestampInterval potential;
            ServerAddress address;

            WriteSetEntry(Key k, Value v, ServerAddress s) : key(k), value(v), address(s) {}
        } WriteSetEntry;

        typedef struct HintSetEntry {
            Key key;
            TimestampInterval interval;
            ServerAddress address;

            HintSetEntry(Key k, TimestampInterval i, ServerAddress a): key(k), interval(i), address(s) {}
        } HintSetEntry;
};

#endif
