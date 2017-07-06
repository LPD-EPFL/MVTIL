/*
 * ServerConnection.cpp
 *
 *  Created on: Feb 11, 2017
 *      Author: wjx
 */

#include "ServerConnection.h"

ServerConnection::ServerConnection(std::string h,int prt):host(h),port(prt) {
	// TODO Auto-generated constructor stub
    boost::shared_ptr<TSocket> s(new TSocket(host, port));
    if(c_stype == NON_BLOCKING_SERVER)
    {
        boost::shared_ptr<TTransport> t(new TFramedTransport(s));
        transport = t;   
    }
    else if(c_stype == BLOCKING_SERVER){
        boost::shared_ptr<TTransport> t(new TBufferedTransport(s));
        transport = t;
    }

    boost::shared_ptr<TProtocol> p(new TBinaryProtocol(transport));
    socket = s;
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

