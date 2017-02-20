//
//  ConfigLoader.h
//  MVCC
//
//  Created by OVSS on 16/02/17.
//  Copyright © 2017 OVSS. All rights reserved.
//

#ifndef _CONFIG_LOADER_H_
#define _CONFIG_LOADER_H_

#include "client.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>

using boost::property_tree::ptree;

class ConfigLoader{
private:
    static std::vector<DataServerInfo> data_server_info;
    static DataServerInfo oracle_info;
    ConfigLoader(){
        const std::string file("config/servers");
        boost::property_tree::ptree pt;
        boost::property_tree::read_xml(file, pt);
        BOOST_FOREACH(ptree::value_type const& v, pt.get_child("DataServers") ) {
            if( v.first == "DataServer" ) {
                DataServerInfo info;
                info.host = v.second.get<std::string>("ip");
                info.port = v.second.get<int>("port");
                data_server_info.push_back(info);
            }
        }

        BOOST_FOREACH(ptree::value_type const&v, pt.get_child("OracleServer"))
        {
            oracle_info.host = v.second.get<std::string>("ip");
            oracle_info.port   = v.second.get<int>("port");
        }
    }
public:
    static std::vector<DataServerInfo> GetDataServerInfo(){
        return data_server_info;
    }
    static DataServerInfo GetOracleServerInfo(){
        return oracle_info;
    }
};

#endif /* ConfigLoader_h */
