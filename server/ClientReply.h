

class ClientReply {
    public:
        ClientReply(TransactionId tid, ReplyType rtype) : tid(tid), rtype(rtype), lockInfo(NULL), value(NULL) {}
        ClientReply(TransactionId tid, ReplyType rtype, LockInfo& lockInfo) : tid(tid), rtype(rtype), lockInfo(lockInfo), value(NULL) {}
        ClientReply(TransactionId tid, ReplyType rtype, LockInfo& lockInfo, Value& value) : tid(tid), rtype(rtype), lockInfo(lockInfo), value(value) {}

        ~ClientReply();
   
    private:
        TransactionId tid;
        ReplyType rtype;
        LockInfo* lockInfo;
        Value* value;
};

