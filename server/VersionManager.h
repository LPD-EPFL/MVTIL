
#ifndef _VERSION_MANAGER_H_
#define _VERSION_MANAGER_H_

#include "DataService_types.h"
#include "SkipList.h"

#define SKIP_LIST_MAXLEVEL 4

typedef skiplist<Timestamp,Value,SKIP_LIST_MAXLEVEL> VersionList;
typedef skiplist_node<Timestamp,Value,SKIP_LIST_MAXLEVEL> Version;
typedef OperationState::type LockState;

// typedef struct LockInfo {
//     LockState state; //the status the operation finished with
//     TimestampInterval locked; //the interval that was successfully locked
//     TimestampInterval potential; //the potential interval that could have been locked had there not been the initial bounds
//     Version* version; //the version that was locked or created
// } LockInfo;

class VersionManager{
	
	public:

		VersionEntry* AddEntry(Key k, VersionManager* ve);
        VersionEntry* CreateNewEntry(Key k);
        VersionEntry* GetVersionList(Key k);

        int RemoveVersion(Key k, Version* v);

		void TryReadLock(Key k, TimestampInterval interval, LockInfo& lockInfo);
        //try to acquire a write lock
        void TryWriteLock(Key k, TimestampInterval interval, LockInfo& lockInfo);
        //get an info on the interval a write lock would be acquired for
        void GetWriteLockHint(Key k, TimestampInterval interval, LockInfo& lockInfo);
        //get a Rw lock
        void TryReadWriteLock(Key k, TimestampInterval interval, LockInfo& lockInfo);
        //try to expand the length of a read lock
        void TryExpandRead(Key k, Timestamp versionTimestamp, TimestampInterval newInterval, LockInfo& lockInfo);
        //try to change the interval of a write lock 
        void TryExpandWrite(Key k, Timestamp versionTimestamp, TimestampInterval newInterval, LockInfo& lockInfo);


	private:

		class VersionEntry{
			//friend class VersionManager;
			private:
               	//Timestamp readMark;
                std::recursive_mutex lock;
           public:
           		Key key;
                VersionList versions;

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

		unordered_map<Key,VersionEntry*> committed_version;
		unordered_map<Key,VersionEntry*> pending_version;
}

#endif