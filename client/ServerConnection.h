/*
 * ServerConnection.h
 *
 *  Created on: Feb 11, 2017
 *      Author: wjx
 */

#ifndef _SERVER_CONNECTION_H_
#define _SERVER_CONNECTION_H_

#include "client.h"
#include "DataService.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <boost/make_shared.hpp>
#include <mutex>

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

class ServerConnection {
    
private:
    boost::shared_ptr<TSocket> socket;
    boost::shared_ptr<TTransport> transport;
    boost::shared_ptr<TProtocol> protocol;
    std::mutex mutex;

public:
	std::string host;
    int port;
	DataServiceClient *client;
    ServerConnection(std::string host,int port);
	virtual ~ServerConnection();
    inline void lock(){
        mutex.lock();
    }
    inline void unlock(){
        mutex.unlock();
    }
};


struct ServerConnectionHasher
{
    friend class ServerConnection;
    std::size_t operator()(const ServerConnection* sc) const
    {
        using std::size_t;
        using std::hash;
        using std::string;
        return (hash<string>()(sc->host+std::to_string(sc->port)));
    }
};

#endif /* CLIENT_SERVERCONNECTION_H_ */
