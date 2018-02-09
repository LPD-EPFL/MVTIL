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
	R_LOCK_SUCCESS = 2,
	W_LOCK_SUCCESS = 3,
	FAIL_PENDING_LOCK = 4,
    FAIL_TIMEOUT = 5,
    COMMIT_OK = 6,
    ABORT_OK = 7,
    ERROR = 8,
}

struct ReadReply {
    1: TransactionId tid,
    2: OperationState state,
    3: Key key,
    4: Value value,   
}

struct WriteReply {
    1: TransactionId tid,
    2: OperationState state,
    3: Key key,
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
    AbortReply HandleAbort(TransactionId tid),

    CommitReply HandleCommit(TransactionId tid),

    ReadReply HandleReadRequest(TransactionId tid, Key k, bool isReadOnly),

    WriteReply HandleWriteRequest(TransactionId tid, Key k, Value v),
}
