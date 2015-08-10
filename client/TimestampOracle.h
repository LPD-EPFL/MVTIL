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

#ifndef _TIMESTAMP_ORACLE_H_
#define _TIMESTAMP_ORACLE_H_

#include <chrono>
#include <ctime>

#include "common.h"
#include "DataServer_types.h"

#define INITIAL_INTERVAL 100
#define INTERVAL_MULTIPLICATION_FACTOR 2
#define INTERVAL_MAX_DURATION 1600

class TimestampOracle {
    private:
        Timestamp initialTimestamp;
        std::chrono::time_point<std::chrono::steady_clock> initialTime;
    public:
        //TimestampOracle(): current(MIN_TIMESTAMP) {}
        TimestampOracle();
        TimestampOracle(Timestamp initial);
        TimestampInterval getInterval(bool isReadOnly, Timespan duration);
        Timestamp getTimestamp();
};

#endif
