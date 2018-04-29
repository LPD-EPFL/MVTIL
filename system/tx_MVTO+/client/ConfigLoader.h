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

#ifndef _CONFIG_LOADER_H_
#define _CONFIG_LOADER_H_

#include "client.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>
#include <iostream>

using namespace std;
using boost::property_tree::ptree;

class ConfigLoader{
private:
    static std::vector<DataServerInfo> data_server_info;
    static DataServerInfo oracle_info;
public:
    ConfigLoader(){
        boost::property_tree::ptree pt;
        boost::property_tree::read_xml("servers.xml", pt);
        BOOST_FOREACH(ptree::value_type const& v, pt.get_child("DataServers") ) {
            if( v.first == "DataServer" ) {
                DataServerInfo info;
                info.host = v.second.get<std::string>("ip");
                info.port = v.second.get<int>("port");
                data_server_info.push_back(info);
            }
        }

        boost::property_tree::read_xml("oracle.xml", pt);
        BOOST_FOREACH(ptree::value_type const&v, pt.get_child("OracleServers"))
        {
            if( v.first == "OracleServer" ) {
                oracle_info.host = v.second.get<std::string>("ip");
                oracle_info.port   = v.second.get<int>("port");
            }
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
