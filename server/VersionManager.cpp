#include "VersionManager.h"


VersionManager::VersionEntry* VersionManager::GetVersionList(Key key){
	if(version_store.count(key) > 0){
		return version_store[key];
	}
	return NULL:
}

void VersionManager::TryReadLock(Key k, TimestampInterval interval, LockInfo& lockInfo)
{
	VersionEntry* ve = committed_version.GetVersionList(k);
	if(ve == NULL){
		lockInfo.state = OperationState::FAIL_NO_VERSION;
		return;
	}

	Version* prev;
    Version* curr;
    curr = ve->versions.find(interval.start, &prev);
    lockInfo.locked.start = prev->key;
	lockInfo.locked.finish = min(interval.finish,curr->key);
	lockInfo.version = prev;
	lockInfo.state = OperationState::R_LOCK_SUCCESS;
}
