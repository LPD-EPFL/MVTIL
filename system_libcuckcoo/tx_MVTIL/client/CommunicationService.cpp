/*   
 *   File: CommunicationService.cpp
 *   Author: Junxiong Wang <junxiong.wang@epfl.ch>
 *   Description: 
 *   CommunicationService.cpp is part of MVTLSYS
 *
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

 #include "CommunicationService.h"

CommunicationService::CommunicationService(){
    for(DataServerInfo info:ConfigLoader::GetDataServerInfo()){
        servers.push_back(new ServerConnection(info.host,info.port));
    }
}

CommunicationService::~CommunicationService(){
    for(ServerConnection* server:servers){
        delete server;
    }
}


ServerConnection* CommunicationService::GetServer(Key key){
    //Hash key based
    size_t hash_value = std::hash<std::string>()(key);
    size_t size = servers.size();
    return servers[hash_value%size];

}

std::vector<ServerConnection*> CommunicationService::GetAllServers(){
    return servers;
}