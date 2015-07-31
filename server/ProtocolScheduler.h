#ifndef _PROTOCOL_SCHEDULER_H_
#define _PROTOCOL_SCHEDULER_H_

#include "DataServer.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>


#include <queue>
#include <iostream>
#include "common.h"
#include "VersionManager.h"
#include "Timer.h"

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using boost::shared_ptr;

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
