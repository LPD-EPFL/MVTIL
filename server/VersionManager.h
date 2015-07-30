
#ifndef _VERSION_MANAGER_H_
#define _VERSION_MANAGER_H_

#include <set>
#include <mutex>
#include <map>
#include <iostream>

#include "Version.h"
#include "VersionSkipList.h"
#include "ClientReply.h"
#include "LockSet.h"
#include "common.h"

class VersionManager {
    private:
       class VersionManagerEntry {
            friend class VersionManager;
            public:
                VersionManagerEntry();
                VersionManagerEntry(Key key);
                VersionManagerEntry(Key key, Version* version);
                VersionManagerEntry(Key key, Timestamp readMark);

                ~VersionManagerEntry();

                Key key;
                VersionSkiplist versions;
                //std::set<Version, versionCompare> versions;

                void purgeVersions(Timestamp barrier);
           private:
                Timestamp readMark;
                std::mutex lock;

           public:
                inline bool isEmpty() {
                    if (versions.size() == 0) return true;
                    return false;
                }

                inline void lockEntry() {
                    lock.lock();
                }

                inline void unlockEntry() {
                    lock.unlock();
                }
 
        };

    public:
        
        VersionManager();
#ifndef INITIAL_TESTING
        VersionManager(Log* log); //recover version manager from existing log
#endif
        ~VersionManager();
    
        //Version getVersion(Key k, TimestampInterval interval, OpType flag);
        
        //try to acquire a read lock
        void tryReadLock(Key k, TimestampInterval interval, LockInfo* lockInfo);
        //try to acquire a write lock
        void tryWriteLock(Key k, TimestampInterval interval, LockInfo* lockInfo);
        //get an info on the interval a write lock would be acquired for
        TimestampInterval getWriteLockHint(Key k, TimestampInterval interval);
        //get a Rw lock
        void tryReadWriteLock(Key k, TimestampInterval interval, LockInfo* lockInfo);

        //remove a version from the versionStore
        int removeVersion(Key k, Version* v);

        //marks for failed reads
        void markReadNotFound(Key k, Timestamp ts);
        Timestamp getMaxReadMark(Key k);
        
        //entry addition and retrieval from the version store
        VersionManagerEntry* addEntry(Key k, VersionManagerEntry* ve);
        VersionManagerEntry* createNewEntry(Key k);
        VersionManagerEntry* getVersionSet(Key k);

        //store version in persistent storage;
        bool persistVersion(Key k, Version* v);
        //updates and repositions a version in the ordered set of per-key versions (only happens when transitioning from PENDING to COMMITTED); also persists the new verion
        bool updateAndPersistVersion(Key k, Version* v, Timestamp new_ts, Timestamp new_duration, Timestamp newPotentialReadMax, OpState new_state);

    private:
#ifndef INITIAL_TESTING
        Log* log;
#endif
        LockSet storeLocks; //TODO inserting does not invalidate iterators in STL containers; what I want to avoid is multiple threads trying to add an entry for the same key at the same time; maybe lock striping would work better than a single lock (with #locks of the same order as the number of concurrent threads) 
        std::map<Key, VersionManagerEntry*> versionStore;

        Timespan getIntersection(Timestamp ts1Left, Timestamp ts1Right, Timestamp ts2Left, Timestamp ts2Right);


};

#endif

