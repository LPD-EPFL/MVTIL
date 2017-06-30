
#ifndef _LOCAL_ORACLE_H_
#define _LOCAL_ORACLE_H_

#include "OracleService.h"
#include "ConfigLoader.h"

#include <chrono>
#include <ctime>

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TBufferTransports.h>

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

using namespace ::Oracle;

#define MAX_FACTOR 32768

class LocalOracle
{
private:
    int64_t client_id;
    int64_t crt;
    std::chrono::time_point<std::chrono::system_clock> initialTime;
public:
    Timestamp GetTimestamp();
    TransactionId GetTransactionId();
    Timestamp GetGlobalTimestamp();
    TransactionId GetGlobalTransactionId();
    LocalOracle(int64_t cid);
    ~LocalOracle();
};

#endif
