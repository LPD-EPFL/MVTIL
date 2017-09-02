
#ifndef _COMMON_H
#define _COMMON_H

#include <string>
#include <cstdint>

#define MIN_TIMESTAMP 0
#define MAX_TIMESTAMP INT64_MAX

typedef int64_t Timestamp;
typedef int64_t TransactionId;

typedef std::string Key;
typedef std::string Value;

#endif
