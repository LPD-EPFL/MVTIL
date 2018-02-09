namespace cpp GC

typedef i64 Timestamp

service GCService {
	GCReply GarbageCollection(Timestamp ts);
}