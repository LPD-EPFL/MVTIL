#ifndef _COMMON_H_
#define _COMMON_H_

#include "DataService.h"
#include <cstdint>
#include <string>
#include <vector>
#include <utility>

typedef int64_t TransactionId;

typedef std::string Key;
typedef std::string Value;

typedef struct DS{
    std::string host;
    int port;
}DataServerInfo;

const std::string NULL_VALUE = "_";

#endif