
#ifndef _VERSION_MANAGER_H_
#define _VERSION_MANAGER_H_

#include "LockManager.h"
//#include "server.h"

//#define MIN_TIMESTAMP 0
//#define MAX_TIMESTAMP 0xFFFF

using namespace std;

// typedef struct LockInfo {
//     LockState state; //the status the operation finished with
//     TimestampInterval locked; //the interval that was successfully locked
//     TimestampInterval potential; //the potential interval that could have been locked had there not been the initial bounds
//     Version* version; //the version that was locked or created
// } LockInfo;

class VersionManager{

    private:

        class VersionEntry{
            //friend class VersionManager;
            //private:
                //Timestamp readMark;
                //std::recursive_mutex lock;
            public:
                Key key;
                VersionList versions;

                //VersionEntry();
                VersionEntry(Key k) : key(k), versions(MIN_TIMESTAMP,MAX_TIMESTAMP){}

                inline bool isEmpty() {
                    if (versions.size() == 0) return true;
                    return false;
                }

                // inline void lockEntry() {
                //     lock.lock();
                // }

                // inline void unlockEntry() {
                //     lock.unlock();
                // }

        };

        unordered_map<Key,VersionEntry*> committed_version;
        unordered_map<Key,LockManager*> locks_manager;
        //unordered_map<Key,VersionEntry*> pending_version;
        //LockManager lock_manager;
	
	public:

		VersionEntry* AddEntry(Key k, VersionManager* ve);
        VersionEntry* CreateNewEntry(Key k);
        VersionEntry* GetVersionList(Key k);

        //int RemoveVersion(Key k, Version* v);

        //Acquire a read lock
		void TryReadLock(TransactionId tid, Key k, TimestampInterval interval, LockInfo& lockInfo);
        //Acquire a write lock
        void TryWriteLock(TransactionId tid, Key k, TimestampInterval interval, LockInfo& lockInfo);
        //Persist a version
        bool UpdateAndPersistVersion(TransactionId tid, Key k, Value value, Timestamp new_ts);
        //Remove a lock
        bool RemoveVersion(Key k, TimestampInterval interval);
        //Remove eariler versions and locks
        bool GarbageCollection(Timestamp time);

};

#endif