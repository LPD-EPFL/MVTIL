#ifndef _SERVER_H_
#define _SERVER_H_

#include "SkipList.h"
#include "DataService_types.h"
#include "config.h"

#include <unordered_set>
#include <unordered_map>
#include <chrono>
#include <thread>
#include <memory>
#include <utility>
#include <tuple>

#define MAX_LEVEL 4

typedef enum {READ, WRITE} LockOperation;
typedef enum {COMMITTED, PENDING, ABORTED} VersionState;

typedef struct VersionEntry
{
	Value version_value;
	VersionState version_state;
	std::set<Timestamp> read_time;
	VersionEntry(){}
	VersionEntry(Value v, VersionState s, std::set<Timestamp> r): version_value(v), version_state(s), read_time(r) {}
}VersionEntry;

typedef skiplist<Timestamp, VersionEntry, MAX_LEVEL> VersionList;//<Write Timestamp, <All read Timestamp, Value, Version State>>
typedef skiplist_node<Timestamp, VersionEntry, MAX_LEVEL> Version;//All the version for a specify key

//Gobal Parameter for Servers
extern uint32_t s_thread_cnt;
extern uint32_t s_port;
extern uint32_t s_timeout;
extern uint32_t s_retry;
extern uint32_t s_type;

#endif
