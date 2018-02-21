/*   
 * Copyright (c) 2017 Junxiong Wang <junxiong.wang@epfl.ch>,
 *                Tudor David <tudor.david@epfl.ch>
 *                Distributed Programming Lab (LPD), EPFL
 *
 * MVTLSYS is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, version 2
 * of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef _LOCAL_ORACLE_H_
#define _LOCAL_ORACLE_H_

#include "OracleService.h"
#include "ConfigLoader.h"

#include <chrono>
#include <ctime>

// #include <thrift/protocol/TBinaryProtocol.h>
// #include <thrift/transport/TSocket.h>
// #include <thrift/transport/TBufferTransports.h>

// using namespace apache::thrift;
// using namespace apache::thrift::protocol;
// using namespace apache::thrift::transport;
//using namespace ::Oracle;

#define MAX_FACTOR 8192

class LocalOracle
{
private:
    int64_t client_id;
    int64_t crt;
    std::chrono::time_point<std::chrono::system_clock> initialTime;
public:
    Timestamp GetTimestamp();
    TransactionId GetTransactionId();
    //Timestamp GetGlobalTimestamp();
    //TransactionId GetGlobalTransactionId();
    LocalOracle(int64_t cid);
    ~LocalOracle();
};

#endif
