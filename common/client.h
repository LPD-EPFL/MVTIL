
#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "DataService_types.h"

using namespace ::TxProtocol;

typedef struct LE 
{
	Value value;
	TimestampInterval interval;
}LockEntry;

typedef struct DS{
    std::string host;
    int port;
}DataServerInfo;

#endif