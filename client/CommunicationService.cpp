
#include "CommunicationService.h"

CommunicationService::CommunicationService(){
    for(DataServerInfo info:ConfigLoader::GetDataServerInfo())
        servers.push_back(new ServerConnection(info.host,info.port));
}

CommunicationService::~CommunicationService(){
    for(ServerConnection* server:servers)
        delete server;
}


ServerConnection* CommunicationService::GetServer(Key key){
    //Hash key based
    size_t hash_value = std::hash<std::string>()(key);
    size_t size = servers.size();
    return servers[hash_value%size];

}
