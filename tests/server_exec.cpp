#include "Scheduler.h"
#include "DataService.h"

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TServer.h>
#include <thrift/server/TThreadPoolServer.h>
#include <thrift/server/TThreadedServer.h>
#include <thrift/server/TNonblockingServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/concurrency/ThreadManager.h>
#include <thrift/concurrency/PosixThreadFactory.h>

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;
using namespace ::apache::thrift::concurrency;

using boost::shared_ptr;
using namespace  ::TxProtocol;

int main(int argc, char **argv) {

  if(argc < 3) 
  {
	cout<<"Please specify the server's port and number of threads!"<<endl;
	return 0;
  }

  int port = atoi(argv[1]);
  int numThreads = atoi(argv[2]);
  boost::shared_ptr<Scheduler> handler(new Scheduler());
  boost::shared_ptr<TProcessor> processor(new DataServiceProcessor(handler));
  boost::shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
  boost::shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
  boost::shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

  boost::shared_ptr<ThreadManager> threadManager = ThreadManager::newSimpleThreadManager(numThreads);
  boost::shared_ptr<ThreadFactory> threadFactory(new PosixThreadFactory());
  threadManager->threadFactory(threadFactory);
  threadManager->start();
  //TThreadPoolServer server(processor, serverTransport, transportFactory, protocolFactory,threadManager);
  //server.serve();

  //Non-blocking Server
  TNonblockingServer server(processor, protocolFactory, port, threadManager);
  server.serve();
  return 0;
}

