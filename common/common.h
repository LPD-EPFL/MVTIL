/* 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/

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
