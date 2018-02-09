
#ifndef _COMMUNICATION_SERVER_
#define _COMMUNICATION_SERVER_

#include "ServerConnection.h"
#include "ConfigLoader.h"
// #include <thrift/protocol/TBinaryProtocol.h>
// #include <thrift/transport/TSocket.h>
// #include <thrift/transport/TBufferTransports.h>

#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/ptree.hpp>

class CommunicationService
{
	public:
		CommunicationService();
		~CommunicationService();
		ServerConnection* GetServer(Key key);
	
	private:
		std::vector<ServerConnection*> servers;
};

#endif
