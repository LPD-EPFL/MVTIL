
#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "DataService_types.h"

using namespace ::TxProtocol;

typedef struct LockEntry 
{
	Value value;
	TimestampInterval interval;
	LockEntry(Value v, TimestampInterval i) : value(v), interval(i) {}
}LockEntry;

typedef struct DS{
    std::string host;
    int port;
}DataServerInfo;

const std::string NULL_VALUE = "_";

#endif