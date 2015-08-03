#ifndef _ROUTING_SERVICE_H_
#define _ROUTING_SERVICE_H_

#include <vector>
#include <thrift/transport/TSocket>

using namespace apache::thrift;

class RoutingService {
    public:
        RoutingService(FILE* configFile); //configFile contains the list of servers
        ~RoutingService();

        shared_ptr<TSocket> getServer(Key k);

    private:
        std::vector<shared_ptr<TSocket>> servers;
};

#endif
