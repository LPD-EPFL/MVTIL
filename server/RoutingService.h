

class ProtocolScheduler {
    public:
        ProtocolScheduler();

        ~ProtocolScheduler();

        int handleRead(ClientId cid, TransactionId tid, TimestampInterval t_interval, Key k);

        int handleWrite(ClientId cid, TransactionId tid, TimestampInterval t_interval, Key k, Value v);

        int handleCommit(ClientId cid, TransactionId tid, Timestamp ts);
    
        int handleAbort(ClientId cid, TransacrionId tid);

    private:
        LockManager* lockManager;
        DataStore* dStore;

}
