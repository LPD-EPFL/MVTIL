#include "Version.h"

Version::Version(Timestamp ts, Timespan d) : state(NO_SUCH_VERSION), timestamp(ts), duration(d), maxReadFrom(ts) {}
Version::Version(Timestamp ts, OpState s) : state(s), timestamp(ts), duration(0), maxReadFrom(ts) {}
Version::Version(Timestamp ts, Timespan d, OpState s, Timestamp mRF) : state(s), timestamp(ts), duration(d), maxReadFrom(mRF) {}


