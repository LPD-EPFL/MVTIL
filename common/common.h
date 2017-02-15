
#ifndef _COMMON_H
#define _COMMON_H

#include <cstdint>
#include <vector>
#include <string>

#define MIN_TIMESTAMP 0
#define MAX_TIMESTAMP INT64_MAX
#define INTERVAL_MAX_DURATION 16

typedef std::uint64_t Timestamp;
typedef std::uint64_t TransactionId;

typedef struct TI{
	Timestamp start;
	Timestamp end;
}TimestampInterval;

typedef std::string Key;
typedef std::string Value;

typedef struct LE 
{
	Value value;
	TimestampInterval interval;
}LockEntry;

typedef struct DS{
    std::string host;
    std::int port;
}DataServerInfo;

#endif
