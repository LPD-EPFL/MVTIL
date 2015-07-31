namespace cpp TxProtocol

typedef i64 TransactionId
typedef i64 Timestamp //make it a string?

struct TimestampInterval {
    1: Timestamp start,
    2: Timestamp finish,
}

enum Operation {
    READ = 1,
    WRITE = 2,
    HINT = 3
}

enum OperationState {
    FAIL_NO_VERSION = 1,
    FAIL_READ_MARK_LARGE = 2,
    FAIL_INTERSECTION_EMPTY = 3,
    FAIL_TIMEOUT = 4,
    WRITES_NOT_FOUND = 5,
    W_LOCK_SUCESS = 6,
    R_LOCK_SUCESS = 7,
    COMMIT_OK = 8,
    ABORT_OK = 9,
    ERROR = 10
}

typedef string Key
typedef string Value

struct ClientGenericRequest {
    1: TransactionId tid,
    2: Operation op,
    3: Timestamp interval,
    4: Key k,
    5: Value v,
}

struct ServerGenericReply {
    1: TransactionId tid,
    2: Operation op,
    3: OperationState state,
    4: TimestampInterval interval,
    5: TimestampInterval potential,
    6: string key,
    7: string value,
}

struct ReadReply {
    1: TransactionId tid,
    2: TimestampInterval interval,
    3: TimestampInterval potential,
    4: OperationState state,
    5: Key k,
    6: Value v,   
}

struct WriteReply {
    1: TransactionId tid,
    2: TimestampInterval interval,
    3: TimestampInterval potential,
    4: OperationState state,
    5: Key k,
}

struct CommitReply {
    1: TransactionId tid,
    2: OperationState state,
}

struct AbortReply {
    1: TransactionId tid,
    2: OperationState state,
}

service DataServer {
    AbortReply handleAbort(TransactionId tid),

    CommitReply handleCommit(TransactionId tid, Timestamp ts),

    ReadReply handleReadRequest(TransactionId tid, TimestampInterval ts, Key k),

    WriteReply handleWriteRequest(TransactionId tid, TimestampInterval ts, Key k, Value v)

    TimestampInterval handleHintRequest(TransactionId tid, TimestampInterval ts, Key k)
 
    ServerGenericReply handleOperation(ClientGenericRequest cr)
   
}
