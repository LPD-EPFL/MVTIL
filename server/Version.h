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

#ifndef _VERSION_H_
#define _VERSION_H_

#include "common.h"
#include "DataServer_types.h"

typedef enum {COMMITTED, PENDING, ABORTED, NO_SUCH_VERSION} OpState;

typedef struct Version {
    OpState state;
    Timestamp timestamp;
    Timespan duration; 
    Timestamp maxReadFrom;

    Version(Timestamp ts, Timespan d) : state(NO_SUCH_VERSION), timestamp(ts), duration(d), maxReadFrom(ts) {}
    Version(Timestamp ts, OpState s) : state(s), timestamp(ts), duration(0), maxReadFrom(ts) {}
    Version(Timestamp ts, Timespan d, OpState s, Timestamp mRF) : state(s), timestamp(ts), duration(d), maxReadFrom(mRF) {}
} Version;

#endif
