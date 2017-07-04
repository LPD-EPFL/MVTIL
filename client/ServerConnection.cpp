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
    boost::shared_ptr<TTransport> t;
    if(c_stype == NON_BLOCKING_SERVER)
    {
        t = boost::make_shared(new TFramedTransport(s));  
    }
    else if(c_stype == BLOCKING_SERVER){
        t = boost::make_shared(new TBufferedTransport(s));
    }
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

