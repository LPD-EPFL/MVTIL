
#ifndef _COMMON_H_
#define _COMMON_H_

#define INITIAL_TESTING 1

#include <cstdint>
#include <string>

typedef uint64_t Timestamp;
typedef uint64_t Timespan;
typedef uint64_t TransactionId;

typedef struct TI{
    Timestamp start;
    Timestamp end;
} TimestampInterval;

typedef std::string Key;
typedef std::string Value;

#endif
