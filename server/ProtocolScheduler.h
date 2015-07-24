#ifndef _PROTOCOL_SCHEDULER_H_
#define _PROTOCOL_SCHEDULER_H_

class ProtocolScheduler {
    public:
        ProtocolScheduler();

        ~ProtocolScheduler();

        ClientReply* handleRead(TransactionId tid, TimestampInterval t_interval, Key k);

        ClientReply* handleWrite(TransactionId tid, TimestampInterval t_interval, Key k, Value v);

        ClientReply* handleCommit(TransactionId tid, Timestamp ts);
    
        ClientReply* handleAbort(TransacrionId tid);

        ClientReply* handleHintRequest(TimestampInterval t_interval, Key k);

        ClientReply* handleSingleKeyOperation(std::string opName, TimestampInterval t_interval, Key k, Value v);

    private:
        VersionManager* versionManager;
        DataStore* dataStore;

        DsKey* toDsKey(Key k, Timestamp ts);
        void abortTransaction(TransactionId tid);

        std::map<TransactionId, std::queue<WSEntry*>> pendingWriteSets;

}
#endif
