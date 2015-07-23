
#ifndef _VERSION_MANAGER_H_
#define _VERSION_MANAGER_H_

#include <set>
#include "version.h"
#include "common.h"

typedef enum {OP_READ, OP_WRITE} op_type;

class VersionManager {
    public:
        VersionManager();
        VersionManager(Log* log); //recover version manager from existing log
        ~VersionManager();

        Version getVersion(Key k, TimestampInterval interval, OpType flag);
        LockInfo* getReadLock(Version& v, TimestampInterval interval);
        LockInfo* getWriteLock(Version& v, TimestampInterval interval);

        void markReadNotFound(Key k, Timestamp ts);
        Timestamp getMaxReadMark(Key k);
        
        int32_t createNewEntry(key k);
        VersionManagerEntry* getVersionsEntry(Key k);

    private:
        Log* log;
        Lock storeLock;//TODO can do better than a global lock; inserting does not invalidate iterators in STL containers; what I want to avoid is multiple threads trying to add an entry for the same key at the same time; maybe lock striping would work better than a single lock (with #locks of the same order as the number of concurrent threads) 
        std::map<Key, VersionManagerEntry*> versionStore;

        void lockStore(); //store should be locked when creating new entries
        void unlockStore();

        add_to_log(LogKey* k, Version* v);

        class VersionManagerEntry {
            friend class VersionManager;
            public:
                VersionManagerEntry();
                VersionManagerEntry(Key key);
                VersionManagerEntry(Key key, Version version);
                VersionManagerEntry(Timestamp readMark);

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

