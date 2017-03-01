/*
 * ServerConnection.cpp
 *
 *  Created on: Feb 11, 2017
 *      Author: wjx
 */

#include "ServerConnection.h"

ServerConnection::ServerConnection(std::string host,int port) {
	// TODO Auto-generated constructor stub
    boost::shared_ptr<TSocket> s(new TSocket(host, port));
    boost::shared_ptr<TTransport> t(new TBufferedTransport(s));
    boost::shared_ptr<TProtocol> p(new TBinaryProtocol(t));
    
    socket = s;
    transport = t;
    protocol = p;

    client = new DataServiceClient(protocol);
    transport->open();//keep the transport to open all the time
}

ServerConnection::~ServerConnection() {
	// TODO Auto-generated destructor stub
    if(transport->isOpen()){
        transport->close();
    }
}

