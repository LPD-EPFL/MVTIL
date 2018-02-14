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

#include "server.h"
#include "Scheduler.h"
#include "DataService.h"

#include <iostream>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TServer.h>
#include <thrift/server/TThreadPoolServer.h>
#include <thrift/server/TNonblockingServer.h>
#include <thrift/server/TThreadedServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/concurrency/ThreadManager.h>
#include <thrift/concurrency/PosixThreadFactory.h>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#define MEASURE_DURATION 3

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;
using namespace ::apache::thrift::concurrency;

using boost::shared_ptr;
using namespace  ::TxProtocol;

void parser_server(int argc, char * argv[]);

void print(const boost::system::error_code& /*e*/,
    boost::asio::deadline_timer* t, Scheduler* scheduler)
{
	int lock_len = 0;
	scheduler->GetSize(lock_len);
	std::cout<<lock_len<<std::endl;
    t->expires_at(t->expires_at() + boost::posix_time::seconds(MEASURE_DURATION));
    t->async_wait(boost::bind(print,
          boost::asio::placeholders::error, t, scheduler));
}



int main(int argc, char **argv) {

	parser_server(argc,argv);

	Scheduler* scheduler = new Scheduler();
	//Async retrieve the version size
	boost::asio::io_service io;
	boost::asio::deadline_timer t(io, boost::posix_time::seconds(MEASURE_DURATION));
	t.async_wait(boost::bind(print,boost::asio::placeholders::error, &t, scheduler));
	boost::thread th(boost::bind(&boost::asio::io_service::run, &io));

	boost::shared_ptr<Scheduler> handler(scheduler);
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

