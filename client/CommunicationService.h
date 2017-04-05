
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
	
	private:
		std::vector<ServerConnection*> servers;
};

#endif
