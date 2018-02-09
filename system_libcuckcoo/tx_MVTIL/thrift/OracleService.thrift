namespace cpp Oracle

typedef i64 TransactionId
typedef i64 Timestamp

service OracleService{
	Timestamp GetTimestamp();
	TransactionId GetTransactionId();
}