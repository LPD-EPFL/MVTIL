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

    Version();
    Version(Timestamp timestamp, Timespan duration);
    Version(Timestamp timestamp, OpState state); //used to create dummmy versions
    Version(Timestamp timestamp, Timespan duration, OpState state, Timestamp maxReadFrom);

} Version;

#endif
