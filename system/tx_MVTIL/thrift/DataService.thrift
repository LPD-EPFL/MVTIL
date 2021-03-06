namespace cpp TxProtocol

typedef i64 TransactionId
typedef i64 Timestamp

typedef string Key
typedef string Value

struct TimestampInterval {
	1: Timestamp lock_start,
	2: Timestamp start,
	3: Timestamp finish,
}

enum Operation {
	READ = 1,
	WRITE = 2,
}

enum OperationState {
	FAIL_NO_VERSION = 1,
	FAIL_READ_MARK_LARGE = 2,
	FAIL_INTERSECTION_EMPTY = 3,
	FAIL_TIMEOUT = 4,
	WRITES_NOT_FOUND = 5,
	R_LOCK_SUCCESS = 6,
	W_LOCK_SUCCESS = 7,
	COMMIT_OK = 8,
	ABORT_OK = 9,
	ERROR = 10 ,
	NOT_IMPLEMENTED = 11,
	FAIL_PENDING_VERSION = 12
}

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
	5: string key,
	6: string value,
}

struct ReadReply {
	1: TransactionId tid,
	2: TimestampInterval interval,
	3: OperationState state,
	4: Key key,
	5: Value value,   
}

struct WriteReply {
	1: TransactionId tid,
	2: TimestampInterval interval,
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

struct GCReply{
	1: OperationState state
} 

service DataService {
	AbortReply HandleAbort(TransactionId tid),

	CommitReply HandleCommit(TransactionId tid, Timestamp ts),

	ReadReply HandleReadRequest(TransactionId tid, TimestampInterval interval, Key k),

	WriteReply HandleWriteRequest(TransactionId tid, TimestampInterval interval, Key k, Value v),

	GCReply GarbageCollection(Timestamp ts)
}