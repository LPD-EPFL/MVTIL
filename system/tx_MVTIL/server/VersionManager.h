/*   
 *   File: VersionManager.h
 *   Author: Junxiong Wang <junxiong.wang@epfl.ch>
 *   Description: 
 *   VersionManager.h is part of MVTLSYS
 *
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

#ifndef _VERSION_MANAGER_H_
#define _VERSION_MANAGER_H_

#include "LockManager.h"
#include "LockSet.h"

#include "tbb/concurrent_hash_map.h"
#include "tbb/tbb.h"

class VersionManager{

	private:

		class VersionEntry{
			//friend class VersionManager;
			private:
				std::mutex lock;
			public:
				Key key;
				VersionList versions;
				LockManager manager;

				//VersionEntry();
				VersionEntry(Key k) : key(k), versions(MIN_TIMESTAMP,MAX_TIMESTAMP), manager(k){}
				~VersionEntry(){}

				inline void lockEntry() {
					lock.lock();
				}

				inline void unlockEntry() {
					lock.unlock();
				}

				inline bool isEmpty() {
					if (versions.size() == 0) return true;
					return false;
				}

		};

		typedef tbb::concurrent_hash_map<Key, std::shared_ptr<VersionEntry>> VersionMap;
		VersionMap all_versions;
		
	public:

		//Acquire a read lock
		void TryReadLock(TransactionId tid, Key k, TimestampInterval interval, LockInfo& lockInfo);
		//Acquire a write lock
		void TryWriteLock(TransactionId tid, Key k, TimestampInterval interval, LockInfo& lockInfo);
		//Persist a version
		bool UpdateAndPersistVersion(TransactionId tid, Key k, Value value, Timestamp old_ts, Timestamp new_ts);
		//Persist read lock
		bool PersistReadLock(TransactionId tid, Key key, Timestamp old_ts, Timestamp new_ts);
		//Remove a lock
		bool RemoveVersion(TransactionId tid, Key k, Timestamp old_ts);
		//Remove eariler versions and locks
		bool GarbageCollection(Timestamp ts);
		//Get size of versions and locks
		bool GetSize(int& version_len, int& lock_len);
};

#endif
