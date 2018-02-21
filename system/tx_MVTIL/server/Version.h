/*   
 * Copyright (c) 2017 Junxiong Wang <junxiong.wang@epfl.ch>,
 *                Tudor David <tudor.david@epfl.ch>
 *                Distributed Programming Lab (LPD), EPFL
 *
 * MVTLSYS is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, version 2
 * of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef _VERSION_H_
#define _VERSION_H_

#include "server.h"

//the state a version object can be in
typedef enum {COMMITTED, PENDING, ABORTED, NO_SUCH_VERSION} OpState;

//the metadata associated with a version
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