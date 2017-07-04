
#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "DataService_types.h"
#include "config.h"

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

//Gobal Parameter for Clients
extern uint32_t c_thread_cnt;
extern uint32_t c_key_space;
extern uint32_t c_interval_duration;
extern uint32_t c_test_type;

#endif