namespace cpp TxProtocol

typedef i64 TransactionId
typedef i64 Timestamp

typedef string Key
typedef string Value

enum Operation {
    READ = 1,
    WRITE = 2,
}

enum OperationState {
    FAIL_NO_VERSION = 1,
    R_SUCCESS = 2,
    R_FAIL_TIMEOUT = 3,
    W_SUCCESS = 4,
    W_FAIL_LATE = 5,
    COMMIT_OK = 6,
    ABORT_OK = 7,
    ERROR = 8,
}

struct ReadReply {
    1: TransactionId tid,
    2: Timestamp ts,
    3: OperationState state,
    4: Key key,
    5: Value value,   
}

struct WriteReply {
    1: TransactionId tid,
    2: Timestamp ts,
    3: OperationState state,
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

service DataService {
    AbortReply HandleAbort(TransactionId tid, Timestamp ts),

    CommitReply HandleCommit(TransactionId tid, Timestamp ts),

    ReadReply HandleReadRequest(TransactionId tid, Timestamp ts, Key k),

    WriteReply HandleWriteRequest(TransactionId tid, Timestamp ts, Key k, Value v),
}