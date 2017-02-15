
#include "CommunicationService.h"

CommunicationService::CommunicationService(){
    for(DataServerInfo info:LoadServerInfo())
        servers.push_back(new ServerConnection(info.host,info.port));
}

CommunicationService::~CommunicationService(){
    for(ServerConnection* server:servers)
        delete server;
}

std::vector<DataServerInfo> CommunicationService::LoadServerInfo(){
    const std::string file("config/servers");
    boost::property_tree::ptree pt;
    boost::property_tree::read_xml(file, pt);
    std::vector<DataServerInfo> ans;
    BOOST_FOREACH(ptree::value_type const& v, pt.get_child("DataServers") ) {
        if( v.first == "DataServer" ) {
            DataServerInfo info;
            info.host = v.second.get<std::string>("ip");
            info.port = v.second.get<std::string>("port");
            ans.push_back(info);
        }
    }
    return ans;
}

ServerConnection* CommunicationService::GetServer(Key key){
    //Hash key based
    size_t hash_value = std::hash<std::string>()(key);
    size_t size = servers.size();
    return servers[hash_value%size];

}
