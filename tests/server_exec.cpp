#include "server.h"
#include "Scheduler.h"
#include "DataService.h"

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TServer.h>
#include <thrift/server/TThreadPoolServer.h>
#include <thrift/server/TNonblockingServer.h>
#include <thrift/server/TThreadedServer.h>
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

void parser_server(int argc, char * argv[]);

int main(int argc, char **argv) {

	parser_server(argc,argv);

	boost::shared_ptr<Scheduler> handler(new Scheduler());
	boost::shared_ptr<TProcessor> processor(new DataServiceProcessor(handler));
	boost::shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());
	boost::shared_ptr<ThreadManager> threadManager = ThreadManager::newSimpleThreadManager(s_thread_cnt);
	boost::shared_ptr<ThreadFactory> threadFactory(new PosixThreadFactory());
	threadManager->threadFactory(threadFactory);
	threadManager->start();
	if(s_type == NON_BLOCKING_SERVER)
	{
		//Non-blocking Server
		TNonblockingServer server(processor, protocolFactory, s_port, threadManager);
		server.serve();
	}
	else if(s_type == BLOCKING_SERVER)
	{
		boost::shared_ptr<TServerTransport> serverTransport(new TServerSocket(s_port));
		boost::shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
		TThreadPoolServer server(processor, serverTransport, transportFactory, protocolFactory,threadManager);
		server.serve();
	}
	return 0;
}

