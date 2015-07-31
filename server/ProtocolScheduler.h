#ifndef _PROTOCOL_SCHEDULER_H_
#define _PROTOCOL_SCHEDULER_H_

#include "DataServer.h"
#include "DataServer_types.h"

#include <queue>
#include <iostream>
#include "common.h"
#include "VersionManager.h"
#include "Timer.h"

using namespace  ::TxProtocol;


#ifdef INITIAL_TESTING
typedef std::string DsKey;
#endif

class ProtocolScheduler : virtual public DataServerIf {
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

        void handleReadRequest(ReadReply& _return, const TransactionId tid, const TimestampInterval& interval, const Key& k);

        void handleWriteRequest(WriteReply& _return, const TransactionId tid, const TimestampInterval& interval, const Key& k, const Value& v);

        void handleHintRequest(TimestampInterval& _return, const TransactionId tid, const TimestampInterval& interval, const Key& k);

        void handleCommit(CommitReply& _return, const TransactionId tid, const Timestamp ts);


        void handleAbort(AbortReply& _return, const TransactionId tid);


        void handleOperation(ServerGenericReply& _return, const ClientGenericRequest& cr);

#ifndef INITIAL_TESTING
        void handleNewEpoch(Timestamp barrier);
#endif

    private:
        VersionManager versionManager;
#ifndef INITIAL_TESTING
        DataStore* dataStore;
#endif
        
        DsKey* toDsKey(Key k, Timestamp ts);

        void abortTransaction(TransactionId tid);

        void garbageCollect(Timestamp barrier);

        std::map<TransactionId, std::queue<WSEntry*>*> pendingWriteSets;

};
#endif
