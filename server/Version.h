
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