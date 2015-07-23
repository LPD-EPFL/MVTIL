
#ifndef _VERSION_MANAGER_H_
#define _VERSION_MANAGER_H_

#include <set>
#include "Version.h"
#include "common.h"

class VersionManager {
    public:
        
        VersionManager();
        VersionManager(Log* log); //recover version manager from existing log
        ~VersionManager();
    
        //trylocks
        //Version getVersion(Key k, TimestampInterval interval, OpType flag);
        LockInfo* tryReadLock(Version& v, TimestampInterval interval);
        LockInfo* tryWriteLock(Version& v, TimestampInterval interval);

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
        LockSet* storeLocks;
        Lock storeLock;//TODO can do better than a global lock; inserting does not invalidate iterators in STL containers; what I want to avoid is multiple threads trying to add an entry for the same key at the same time; maybe lock striping would work better than a single lock (with #locks of the same order as the number of concurrent threads) 
        std::map<Key, VersionManagerEntry*> versionStore;

        void lockStore(); //store should be locked when creating new entries
        void unlockStore();

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
                Lock lock;
        }

    struct versionCompare {
         bool operator() (const Version& v1, const Version& v2 const) {
             return v1.timestamp < v2.timestamp;
         }
    }

};

#endif

