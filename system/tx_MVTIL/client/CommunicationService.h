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

#ifndef _COMMUNICATION_SERVER_
#define _COMMUNICATION_SERVER_

#include "client.h"
#include "ServerConnection.h"
#include "ConfigLoader.h"

#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/ptree.hpp>

class CommunicationService
{
    public:
        CommunicationService();
        ~CommunicationService();
        ServerConnection* GetServer(Key key);
        std::vector<ServerConnection*> GetAllServers();
    
    private:
        std::vector<ServerConnection*> servers;
};

#endif
