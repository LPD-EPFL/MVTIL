// This autogenerated skeleton file illustrates how to build a server.
// You should copy it to another filename to avoid overwriting it.

#include "DataService.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using boost::shared_ptr;

using namespace  ::TxProtocol;

class DataServiceHandler : virtual public DataServiceIf {
 public:
  DataServiceHandler() {
    // Your initialization goes here
  }

  void HandleAbort(AbortReply& _return, const TransactionId tid, const Timestamp ts) {
    // Your implementation goes here
    printf("HandleAbort\n");
  }

  void HandleCommit(CommitReply& _return, const TransactionId tid, const Timestamp ts) {
    // Your implementation goes here
    printf("HandleCommit\n");
  }

  void HandleReadRequest(ReadReply& _return, const TransactionId tid, const Timestamp ts, const Key& k) {
    // Your implementation goes here
    printf("HandleReadRequest\n");
  }

  void HandleWriteRequest(WriteReply& _return, const TransactionId tid, const Timestamp ts, const Key& k, const Value& v) {
    // Your implementation goes here
    printf("HandleWriteRequest\n");
  }

};

// int main(int argc, char **argv) {
//   int port = 9090;
//   shared_ptr<DataServiceHandler> handler(new DataServiceHandler());
//   shared_ptr<TProcessor> processor(new DataServiceProcessor(handler));
//   shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
//   shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
//   shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

//   TSimpleServer server(processor, serverTransport, transportFactory, protocolFactory);
//   server.serve();
//   return 0;
// }

