#ifndef _CLIENT_REPLY_H_
#define _CLIENT_REPLY_H_

#include "common.h"
#include "version.h"

typedef enum {FAIL_NO_VERSION, FAIL_READ_MARK_LARGE, FAIL_INTERSECTION_EMPTY, WRITES_NOT_FOUND, W_LOCK_SUCCESS, R_LOCK_SUCCESS} lockState;

typedef struct LockInfo {
    lockState state;
    TimestampInterval locked;
    TimestampInterval potential;
    const Version* version;
} LockInfo;

typedef enum {READ_REPLY, WRITE_REPLY, COMMIT_ACK, ABORT_ACK, HINT_REPLY, TIMEOUT} ReplyType;

class ClientReply {
    public:
        ClientReply(TransactionId tid, ReplyType rtype) : tid(tid), rtype(rtype), lockInfo(NULL), value(NULL) {}
        ClientReply(TransactionId tid, ReplyType rtype, LockInfo* lockInfo) : tid(tid), rtype(rtype), lockInfo(lockInfo), value(NULL) {}
        ClientReply(TransactionId tid, ReplyType rtype, LockInfo* lockInfo, Value* value) : tid(tid), rtype(rtype), lockInfo(lockInfo), value(value) {}

        ~ClientReply();
   
    private:
        TransactionId tid;
        ReplyType rtype;
        LockInfo* lockInfo;
        Value* value;
};

#endif

