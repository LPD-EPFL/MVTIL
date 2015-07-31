
#ifndef _COMMON_H_
#define _COMMON_H_

#define INITIAL_TESTING 1

#include <cstdint>
#include <string>
#include "DataServer_types.h"

using namespace TxProtocol;

//typedef uint64_t Timestamp;
typedef int64_t Timespan;
//typedef uint64_t TransactionId;

#define MIN_TIMESTAMP INT64_MIN
#define MAX_TIMESTAMP (INT64_MAX-1)

//typedef struct TI{
    //Timestamp start;
    //Timestamp end;
//} TimestampInterval;

//typedef std::string Key;
//typedef std::string Value;

#endif
