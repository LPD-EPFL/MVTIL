// This autogenerated skeleton file illustrates how to build a server.
// You should copy it to another filename to avoid overwriting it.

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

class OracleServiceHandler : virtual public OracleServiceIf {
    
  private:
    TransactionId unique_id;
    std::chrono::time_point<std::chrono::system_clock> initialTime;
    
 public:
  OracleServiceHandler() {
    // Your initialization goes here
    initialTime = std::chrono::system_clock::now();
    unique_id = 1;
  }

  Timestamp GetTimestamp() {
    // Your implementation goes here
    printf("GetTimestamp\n");
    Timestamp duration = ((std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() -initialTime).count())<<10) + (unique_id)%1024;
    return duration;
  }

  TransactionId GetTransactionId() {
    // Your implementation goes here
    printf("GetTransactionId\n");
    unique_id++;
    return unique_id;
  }

};

// int main(int argc, char **argv) {
//   int port = 9090;
//   shared_ptr<OracleServiceHandler> handler(new OracleServiceHandler());
//   shared_ptr<TProcessor> processor(new OracleServiceProcessor(handler));
//   shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
//   shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
//   shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

//   TSimpleServer server(processor, serverTransport, transportFactory, protocolFactory);
//   server.serve();
//   return 0;
// }

