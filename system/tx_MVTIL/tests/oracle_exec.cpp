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

#include "OracleService.h"
#include <chrono>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using boost::shared_ptr;

using namespace  ::Oracle;

class OracleHandler : virtual public OracleServiceIf {
    
  private:
    TransactionId unique_id;
    std::chrono::time_point<std::chrono::system_clock> initialTime;
    
 public:
  OracleHandler() {
    // Your initialization goes here
    initialTime = std::chrono::system_clock::now();
    unique_id = 0;
  }

  Timestamp GetTimestamp() {
    // Your implementation goes here
    Timestamp duration = ((std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() -initialTime).count()) << 5) + unique_id;
    return duration;
  }

  TransactionId GetTransactionId() {
    // Your implementation goes here
    unique_id++;
    return unique_id;
  }

};

int main(int argc, char **argv) {

  if(argc < 2) 
  {
    printf("Please specify the oracle's port!");
    return 0;
  }

  int port = atoi(argv[1]);
  shared_ptr<OracleHandler> handler(new OracleHandler());
  shared_ptr<TProcessor> processor(new OracleServiceProcessor(handler));
  shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
  shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
  shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

  TSimpleServer server(processor, serverTransport, transportFactory, protocolFactory);
  server.serve();
  return 0;
}