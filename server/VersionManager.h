
#ifndef _VERSION_MANAGER_H_
#define _VERSION_MANAGER_H_

#include <set>
#include <mutex>

#include "Version.h"
#include "LockSet.h"
#include "common.h"

class VersionManager {
    public:
        
        VersionManager();
        VersionManager(Log* log); //recover version manager from existing log
        ~VersionManager();
    
        //Version getVersion(Key k, TimestampInterval interval, OpType flag);
        
        //try to acquire a read lock
        void tryReadLock(Key k, TimestampInterval interval, LockInfo& lockInfo);
        //try to acquire a write lock
        void tryWriteLock(Key k, TimestampInterval interval, LockInfo& lockInfo);
        //get an info on the interval a write lock would be acquired for
        TimestampInterval getWriteLockHint(Key k, TimestampInterval interval);
        //get a Rw lock
        LockInfo* tryReadWriteLock(Key k, TimestampInterval interval, LockInfo& lockInfo);

        //marks for failed reads
        void markReadNotFound(Key k, Timestamp ts);
        Timestamp getMaxReadMark(Key k);
        
        //entry addition and retrieval from the version store
        VersionManagerEntry* addEntry(Key k, VersionManagerEntry* ve);
        VersionManagerEntry* createNewEntry(Key k);
        VersionManagerEntry* getVersionSet(Key k);

        //store version in persistent storage;
        bool persistVersion(Key k, Version& v);

    private:
        Log* log;
        LockSet storeLocks; //TODO inserting does not invalidate iterators in STL containers; what I want to avoid is multiple threads trying to add an entry for the same key at the same time; maybe lock striping would work better than a single lock (with #locks of the same order as the number of concurrent threads) 
        std::map<Key, VersionManagerEntry*> versionStore;

        //add_to_log(LogKey* k, Version* v);

        class VersionManagerEntry {
            friend class VersionManager;
            public:
                VersionManagerEntry();
                VersionManagerEntry(Key key);
                VersionManagerEntry(Key key, Version version);
                VersionManagerEntry(Key key, Timestamp readMark);

                Key key;
                std::set<Version, versionCompare> versions;

                bool isEmpty();
                void purgeVersions(Timestamp barrier);

                void lockEntry();
                void unlockEntry();

            private:
                Timestamp readMark;
                std::mutex lock;
        }

    struct versionCompare {
         bool operator() (const Version& v1, const Version& v2 const) {
             return v1.timestamp < v2.timestamp;
         }
    }

};

#endif

