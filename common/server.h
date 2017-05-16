#ifndef _SERVER_H_
#define _SERVER_H_

#include "SkipList.h"
#include "DataService_types.h"
#include "CapabilityOperation.h"

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

#endif