
#ifndef _COMMON_H_
#define _COMMON_H_

#define INITIAL_TESTING 1

#include <cstdint>
#include <string>

typedef uint64_t Timestamp;
typedef uint64_t Timespan;
typedef uint64_t TransactionId;

#define MIN_TIMESTAMP 0
#define MAX_TIMESTAMP UINT64_MAX

typedef struct TI{
    Timestamp start;
    Timestamp end;
} TimestampInterval;

typedef std::string Key;
typedef std::string Value;

#endif
