
#include "LocalOracle.h"

LocalOracle::LocalOracle(int64_t cid):client_id(cid){
	crt = 1;
	std::tm tm = {};
	strptime("Sun Jan 1 2017 00:00:00", "%a %b %d %Y %H:%M:%S", &tm);
	initialTime = std::chrono::system_clock::from_time_t(std::mktime(&tm));
}

LocalOracle::~LocalOracle(){
}

Timestamp LocalOracle::GetTimestamp(){
	Timestamp duration = ((std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - initialTime).count()) << 10) + (client_id << 5)+ crt;
	return duration;
}

TransactionId LocalOracle::GetTransactionId(){
	TransactionId tid = client_id * MAX_FACTOR + crt;
	crt = (crt+1) % MAX_FACTOR;
	return tid;
}

Timestamp LocalOracle::GetGlobalTimestamp(){
	DataServerInfo info = ConfigLoader::GetOracleServerInfo();
	boost::shared_ptr<TSocket> socket(new TSocket(info.host, info.port));
	boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
	boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
	
	transport->open();
	Oracle::OracleServiceClient client(protocol);
	Timestamp ts =  client.GetTimestamp();
	transport->close();
	return ts;
}

Timestamp LocalOracle::GetGlobalTransactionId(){
	DataServerInfo info = ConfigLoader::GetOracleServerInfo();
	boost::shared_ptr<TSocket> socket(new TSocket(info.host, info.port));
	boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
	boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
	
	transport->open();
	Oracle::OracleServiceClient client(protocol);
	TransactionId tid =  client.GetTransactionId();
	transport->close();
	return tid;
	
}
