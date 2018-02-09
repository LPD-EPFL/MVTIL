#include "CommunicationService.h"
#include "LocalOracle.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TBufferTransports.h>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#define TD 30*1000
#define GC_DURATION 60

class GarbageCollector
{
	private:
		CommunicationService service;
		LocalOracle oracle;

	public:
		GarbageCollector():oracle(0){

		}

		~GarbageCollector(){
			
		}

		void InvokeGC(){
			Timestamp ts = oracle.GetTimestamp() - (TD << LOW_BITE);
			std::vector<ServerConnection*> servers = service.GetAllServers();
			for(ServerConnection* server:servers){
				GCReply reply;
				server->client->GarbageCollection(reply, ts);
			}
		}
};

void PeriodicGC(const boost::system::error_code& /*e*/,
    boost::asio::deadline_timer* t, GarbageCollector* gc){
    gc->InvokeGC();
    t->expires_at(t->expires_at() + boost::posix_time::seconds(GC_DURATION));
    t->async_wait(boost::bind(PeriodicGC,boost::asio::placeholders::error, t, gc));
}

int main(int argc, char **argv) {
	GarbageCollector gc;
	boost::asio::io_service io;
	boost::asio::deadline_timer t(io, boost::posix_time::seconds(GC_DURATION));
	t.async_wait(boost::bind(PeriodicGC,boost::asio::placeholders::error, &t, &gc));
	io.run();
	return 0;
}
