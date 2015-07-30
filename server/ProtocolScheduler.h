#ifndef _PROTOCOL_SCHEDULER_H_
#define _PROTOCOL_SCHEDULER_H_

#include <queue>
#include <iostream>
#include "common.h"
#include "ClientReply.h"
#include "VersionManager.h"

#ifdef INITIAL_TESTING
typedef std::string DsKey;
#endif

class ProtocolScheduler {
    private:
        typedef struct WSEntry{
            Version *version;
            Key key;
            Value value;

            WSEntry(Version* ver, Key k, Value val) : version(ver), key(k), value(val) {}
            WSEntry() : version(), key(), value() {}
        } WSEntry;
    public:
        ProtocolScheduler();

        ~ProtocolScheduler();

        ClientReply* handleRead(TransactionId tid, TimestampInterval t_interval, Key k);

        ClientReply* handleWrite(TransactionId tid, TimestampInterval t_interval, Key k, Value v);

        ClientReply* handleCommit(TransactionId tid, Timestamp ts);
    
        ClientReply* handleAbort(TransactionId tid);

        ClientReply* handleHintRequest(TimestampInterval t_interval, Key k);

        ClientReply* handleSingleKeyOperation(TransactionId tid, std::string opName, TimestampInterval t_interval, Key k, Value v);

    private:
        VersionManager versionManager;
#ifndef INITIAL_TESTING
        DataStore* dataStore;
#endif
        
        DsKey* toDsKey(Key k, Timestamp ts);

        void abortTransaction(TransactionId tid);

        std::map<TransactionId, std::queue<WSEntry*>*> pendingWriteSets;

};
#endif
