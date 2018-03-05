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

#include "LocalOracle.h"

LocalOracle::LocalOracle(int64_t cid):client_id(cid){
    crt = 1;
    std::tm tm = {};
    strptime("Wed Sept 20 2017 00:00:00", "%a %b %d %Y %H:%M:%S", &tm);
    initialTime = std::chrono::system_clock::from_time_t(std::mktime(&tm));
}

LocalOracle::~LocalOracle(){
}

Timestamp LocalOracle::GetTimestamp(){
    Timestamp start_ts = ((std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - initialTime).count()) << LOW_BITE) + (client_id << 10) + crt % 1024;
    return start_ts;
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
