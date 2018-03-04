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

#include "ServerConnection.h"

ServerConnection::ServerConnection(std::string h,int prt):host(h),port(prt) {
    boost::shared_ptr<TSocket> s(new TSocket(host, port));
    boost::shared_ptr<TTransport> t(new TFramedTransport(s));
    boost::shared_ptr<TProtocol> p(new TBinaryProtocol(t));
    
    socket = s;
    transport = t;
    protocol = p;

    client = new DataServiceClient(protocol);
    transport->open(); //keep the transport to open all the time
}

ServerConnection::~ServerConnection() {
    if(transport->isOpen()){
        transport->close();
    }
}

