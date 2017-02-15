/*
 * ServerConnection.h
 *
 *  Created on: Feb 11, 2017
 *      Author: wjx
 */

#ifndef _SERVER_CONNECTION_H_
#define _SERVER_CONNECTION_H_

#include "common.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TBufferTransports.h>

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

class ServerConnection {
    
private:
    boost::shared_ptr<TSocket> socket;
    boost::shared_ptr<TTransport> transport;
    boost::shared_ptr<TProtocol> protocol;
public:
    ServerConnection(std::string host,int port);
	virtual ~ServerConnection();
};

#endif /* CLIENT_SERVERCONNECTION_H_ */
