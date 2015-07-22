

class ProtocolScheduler {
    public:
        ProtocolScheduler();

        ~ProtocolScheduler();

        ClientReply* handleRead(TransactionId tid, TimestampInterval t_interval, Key k);

        ClientReply* handleWrite(TransactionId tid, TimestampInterval t_interval, Key k, Value v);

        ClientReply* handleCommit(TransactionId tid, Timestamp ts);
    
        ClientReply* handleAbort(TransacrionId tid);

    private:
        VersionManager* versionManager;
        DataStore* dataStore;

        DsKey* toDsKey(Key k, Timestamp ts);
        void abortTransaction(TransactionId tid);

        std::map<TransactionId, std::queue<WSEntry*>> pendingWriteSets;

}
