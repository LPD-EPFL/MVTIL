#include "Scheduler.h"
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

int main(int argc, char **argv) {

  if(argc < 2) 
  {
  	cout<<"Please specify the server's port!"<<endl;
  	return 0;
  }

  int port = atoi(argv[1]);
  boost::shared_ptr<Scheduler> handler(new Scheduler());
  boost::shared_ptr<TProcessor> processor(new DataServiceProcessor(handler));
  boost::shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
  boost::shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
  boost::shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

  TSimpleServer server(processor, serverTransport, transportFactory, protocolFactory);
  server.serve();
  return 0;
}

