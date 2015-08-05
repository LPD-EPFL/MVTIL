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
    W_LOCK_SUCCESS = 6,
    R_LOCK_SUCCESS = 7,
    COMMIT_OK = 8,
    ABORT_OK = 9,
    ERROR = 10 ,
    NOT_IMPLEMENTED = 11,
    FAIL_PENDING_VERSION = 12
}

typedef string Key
typedef string Value

struct ClientGenericRequest {
    1: TransactionId tid,
    2: Operation op,
    3: Timestamp interval,
    4: Key key,
    5: Value value,
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
    5: Key key,
    6: Value value,   
}

struct WriteReply {
    1: TransactionId tid,
    2: TimestampInterval interval,
    3: TimestampInterval potential,
    4: OperationState state,
    5: Key key,
}

struct HintReply {
    1: TransactionId tid,
    2: TimestampInterval interval,
    3: TimestampInterval potential,
    4: Key key,
}

struct CommitReply {
    1: TransactionId tid,
    2: OperationState state,
}

struct AbortReply {
    1: TransactionId tid,
    2: OperationState state,
}

struct ExpandReadReply {
    1: TransactionId tid,
    2: OperationState state,
    3: TimestampInterval interval,
}

struct ExpandWriteReply {
    1: TransactionId tid,
    2: OperationState state,
    3: TimestampInterval interval,
}

service DataServer {
    AbortReply handleAbort(TransactionId tid),

    CommitReply handleCommit(TransactionId tid, Timestamp ts),

    ReadReply handleReadRequest(TransactionId tid, TimestampInterval interval, Key k, bool isReadOnly),

    WriteReply handleWriteRequest(TransactionId tid, TimestampInterval interval, Key k, Value v)

    HintReply handleHintRequest(TransactionId tid, TimestampInterval interval, Key k)
 
    ServerGenericReply handleOperation(ClientGenericRequest cr)

    ExpandReadReply handleExpandRead(TransactionId tid, TimestampInterval newInterval, Key k)

    ExpandWriteReply handleExpandWrite(TransactionId tid, TimestampInterval newInterval, Key k)
   
}
