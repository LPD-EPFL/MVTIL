#ifndef _SERVER_H_
#define _SERVER_H_

#include "SkipList.h"
#include "DataService_types.h"
#include "config.h"

#include <unordered_map>

#define MAX_LEVEL 4

using namespace TxProtocol;

typedef OperationState::type LockState;
typedef skiplist<Timestamp,Value,MAX_LEVEL> VersionList;
typedef skiplist_node<Timestamp,Value,MAX_LEVEL> Version;

typedef struct LockInfo {
    LockState state; //the status the operation finished with
    TimestampInterval locked; //the interval that was successfully locked
    Version* version; //the version that was locked or created
} LockInfo;

//Gobal Parameter for Servers
extern uint32_t s_thread_cnt;
extern uint32_t s_port;
extern uint32_t s_timeout;
extern uint32_t s_retry;
extern uint32_t s_policy;
extern uint32_t s_type;

#endif