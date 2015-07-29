#include "VersionManager.h"

VersionManager::VersionManager() {
}

#ifndef INITIAL_TESTING
VersionManager::VersionManager(Log* l) {
    //TODO: create the versions data structure from the log
}
#endif

VersionManager::~VersionManager() {
    //TODO iterate through the version store and delete everything there (in particular call delete on the version entries)
}

void VersionManager::markReadNotFound(Key k, Timestamp ts) {
    VersionManagerEntry* ve = getVersionSet(k);
    VersionManagerEntry* ret;
    if (ve == NULL) {
        ve = new VersionManagerEntry(k, ts);
        ret = addEntry(k,ve);
        if (ret == ve) {
             return;
        }
        delete ve; 
        ve = ret;
    }
    if (ve->readMark < ts) {
        ve->readMark = ts;
    }
}

Timestamp VersionManager::getMaxReadMark(Key k) {
    VersionManagerEntry* ve = getVersionSet(k);
    if (ve == NULL) {
        return MIN_TIMESTAMP;
    }
    return ve->readMark;
}

VersionManager::VersionManagerEntry* VersionManager::getVersionSet(Key k) {
    std::map<Key, VersionManagerEntry*>::iterator it = versionStore.find(k);
    if (it == versionStore.end()) {
        return NULL;
    }
    return it->second; 
}

VersionManager::VersionManagerEntry* VersionManager::addEntry(Key k, VersionManagerEntry* ve) {
     storeLocks.lock(k);
     std::pair<std::map<Key,VersionManagerEntry*>::iterator,bool> ret = versionStore.insert(std::pair<Key,VersionManagerEntry*>(k,ve));
     if (ret.second == false) {
        ve = ret.first->second;
     }
     storeLocks.unlock(k);
     return ve;
}

VersionManager::VersionManagerEntry* VersionManager::createNewEntry(Key k) {
     storeLocks.lock(k);
     VersionManagerEntry* ve = new VersionManagerEntry(k);
     std::pair<std::map<Key,VersionManagerEntry*>::iterator,bool> ret = versionStore.insert(std::pair<Key,VersionManagerEntry*>(k,ve));
     if (ret.second == false) {
        delete ve;
        ve = ret.first->second;
     }
     storeLocks.unlock(k);
     return ve;
}


bool VersionManager::updateAndPersistVersion(Key k, Version* v, Timestamp new_ts, Timestamp new_duration, Timestamp newPotentialReadMax, state new_state) {

    VersionManagerEntry* ve = getVersionSet(key);
    ve->lockEntry();
    Timestamp old_ts = v->timestamp;
    v->duration = new_duration;
    if (v->maxReadFrom < newPotentialReadMax) {
        v->maxReadFrom = newPotentialReadMax;
    }
    v->state = new_state;
     
    if (old_ts != new_ts) {
        v->timestamp = new_ts;
        ve->versions.reposition(old_ts);
    }

    ve->unlockEntry();
    //TODO: add version to RocksDB
    return true;
}
bool VersionManager::persistVersion(Key k, Version* v) {
    //TODO: add version to RocksDB
    return true;
}


void VersionManager::tryReadLock(Key key, TimestampInterval interval, LockInfo* lockInfo) {
    VersionManagerEntry* ve = getVersionSet(key);
    if (ve == NULL) {
        markReadNotFound(key, interval.end); //be conservative
        lockInfo->state = FAIL_NO_VERSION;
        return;
    }
    ve->lockEntry();
    Version* selected_version = NULL;
    Timestamp next_timestamp = 0;

    OrderedSetNode* prev = NULL;
    OrderedSetNode* node;
    node = ve->versions.find(interval.start);

    if (node->getTimestamp() != interval.start) {
        if (pred->getVersion() != NULL) {
            node = pred;
        }
    }
    
    OrderedSetNode* next= node;
    Version *ver = node->getVersion();

    while ((ver != NULL) && (ver->timestamp <= interval.end)) {
        next = node->getNext();
        if (ver->state == COMMITTED) {
            if (selected_version == NULL) {
                selected_version = ver;
                next_timestamp = next->getTimestamp();
            } else {
                if (getIntersection(ver->timestamp,next->getTimestamp(),interval.start,interval.end) > getIntersection(selected_version->timestamp, next_timestamp, interval.start, interval.end)) {
                    selected_version = ver;
                    next_timestamp = next->getTimestamp();
                }
            }
        }
        node=node->getNext();
        ver = node->getVersion();
    }
    if (selected_version == NULL) {
        markReadNotFound(key, interval.end);
        lockInfo->state = FAIL_NO_VERSION;
        ve->unlockEntry();
        return;
    }


    lockInfo->locked.start = selected_version->timestamp;
    lockInfo->locked.end = std::min(next_timestamp, interval.end);
    lockInfo->potential.start = selected_version->timestamp;
    lockInfo->potential.end = next_timestamp;
    lockInfo->version = selected_version;
    lockInfo->state = R_LOCK_SUCCESS;

    if (selected_version->maxReadFrom < lockInfo->locked.end) {
        selected_version->maxReadFrom = lockInfo->locked.end;
    }
    
    ve->unlockEntry();
    return;

}

void VersionManager::tryWriteLock(Key key, TimestampInterval interval, LockInfo* lockInfo) {
    VersionManagerEntry* ve = getVersionSet(key);
    if (ve == NULL) {
        ve = createNewEntry(key);
    }
    ve->lockEntry();
    if (ve->isEmpty() || (ve->versions.getFirstTimestamp() > interval.end)) {
        if (getMaxReadMark(key) > interval.end) {
            lockInfo->state = FAIL_READ_MARK_LARGE;    
            lockInfo->potential.start = getMaxReadMark(key);
            ve->unlockEntry();
            return;
        }
        
        Timestamp start = std::max(interval.start, getMaxReadMark(key));
        if (ve->isEmpty()) {
            lockInfo->potential.end = MAX_TIMESTAMP;
        } else {
            lockInfo->potential.end = ve->versions.getFirstTimestamp();
        }
        Version* new_version = new Version(start, interval.end - start, PENDING, start); //TODO what should I set maxReadFrom to here?
        ve->versions.insert(start, new_version); 
        lockInfo->locked.start = start;
        lockInfo->locked.end = interval.end;
        lockInfo->potential.start = getMaxReadMark(key); 
        lockInfo->version = new_version;
        lockInfo->state = W_LOCK_SUCCESS;
        ve->unlockEntry();
        return;
    }

    Version* selected_version = NULL;   
    Version* selected_pending = NULL;


    std::set<Version>::iterator it;
    it = ve->versions.lower_bound(interval.start); //Points to the first entry to be considered

    OrderedSetNode* node;
    OrderedSetNode* prev;
    node = ve->versions.find(interval.start, prev);

    if (node->getTimestamp() != interval.start) {
        if (prev->getVersion() != NULL) {
            node = prev;
        }
    }
//    if (it->timestamp != interval.start) {
//        if (it != ve->versions.start()) {
//            --it; //TODO: check I am not doing -- on the first element of the set; if it is the first element of the set, I need to compare with the read mark;
        } 
        //else {
            //if (getMaxReadMark(key) > interval.end) {
                //Version* dummy = new Version(getMaxReadMark(key), NO_SUCH_VERSION);
                //return dummy;
                
            //}
            //if (dummy->timestamp > interval.start) {
                //interval.start = dummy->timestamp; //TODO is this necessary? do I have to change anything if this happens?
            //}
        //}
    }

    Interval candidate;
    Interval candidate_pending;

    OrderedSetNode* next = node;
    Version* ver = node->getVersion();

    Timestamp next_timestamp = 0;
    Timestamp next_timestamp_pending = 0;

    while ((ver != NULL) && (node->getTimestamp() <= interval.end)) { //TODO + duration
        next = node->getNext();
        if (ver->state == COMMITTED) {
            if ((next->getTimestamp() - ver->maxReadFrom) > (candidate.end - candidate.start)) {
                candidate.start = ver->maxReadFrom;
                candidate.end = next->getTimestamp();
            }
            if ((selected_version == NULL) && (ver->maxReadFrom < interval.end)){
                selected_version =  ver;
                next_timestamp = next->getTimestamp();
            } else {
                if (getIntersection(ver->maxReadFrom, next->getTimestamp(), interval.start, interval.end) > getIntersection(selected_version->maxReadFrom, next_timestamp, interval.start, interval.end)) {
                    selected_version = ver;
                    next_timestamp = next->getTimestamp();
                }
            }
        } else if (ver->state == PENDING) {
            if ((next->getTimestamp() - (ver->timestamo + ver->duration)) > (candidate_pending.end - candidate_pending.start)) {
               candidate_pending.start = ver->timestamp + it->duration;  
               candidate_pending.end = next->getTimestamp();
            }
            if ((selected_pending == NULL) && ((ver->timestamp + ver->duration) < interval.end)){
                selected_pending = ver;
                next_timestamp_pending = next->getTimestamp();
            } else {
                if (getIntersection(ver->timestamp + ver->duration, next->getTimestamp(), interval.start, interval.end) > getIntersection(selected_pending->timestamp + selected_pending->duration, next_timestamp_pending, interval.start, interval.end)) {
                    selected_pending=it;
                    next_timestamp_pending = next->getTimestamp();
                }
            }
        }
        node = node->getNext();
        ver = node->getVersion();
    }
    if (selected_version != NULL) {
        Timestamp start = max(interval.start, selected_version->maxReadFrom);
        Timestamp end = min(interval.end, next_timestamp);
        Version* new_version = new Version(start, interval.end - start, PENDING, start); //TODO what should I set maxReadFrom to here?
        ve->insert(start, new_version); 
        lockInfo->version = new_vesion;
        lockInfo->locked.start = start;
        lockInfo->locked.end = end;
        lockInfo->potential.start = selected_version->maxReadFrom;
        lockInfo->potential.end = next_timestamp;
        lockInfo->state = W_LOCK_SUCESS;
        ve->unlockEntry();
        return; 
    }
    if (selected_pending != NULL) {
        Timestamp start = max(interval.start, selected_pending->start + selected_pending->duration);
        Timestamp end = min(interval.end, next_timestamp_pending);
        Version* new_version = new Version(start, interval.end - start, PENDING, start); //TODO what should I set maxReadFrom to here?
        ve->insert(start, new_version); 
        lockInfo->vesion = new_version;
        lockInfo->locked.start = start;
        lockInfo->locked.end = end;
        lockInfo->potential.start = selected_pending->start + selected_pending->duration;
        lockInfo->potential.end = next_timestamp_pending;
        lockInfo->state = W_LOCK_SUCCESS;
        ve->unlockEntry();
        return;
    }
    lockInfo->state = FAIL_INTERSECTION_EMPTY;
    if (candidate.start != TIMESTAMP_MIN) {
        lockInfo->potential.start = candidate.start;
        lockInfo->potential.end = candidate.end;
    } else if (candidate_pending.start != TIEMSTAMP_MIN) {
        lockInfo->potential.start = candidate_pending.start;
        lockInfo->potential.end = candidate_pending.end;
    } else {
        lockInfo->potential.start = TIMESTAMP_MIN;
        lockInfo->potential.end = TIMESTAMP_MIN;
    }
    ve->unlockEntry();
    return;

}

TimestampInterval VersionManager::tryWriteLockHint(Key k, TimestampInterval interval) {
    VersionManagerEntry* ve = getVersionSet(key);
    TimestampInterval ret;
    ret.start=MIN_TIMESTAMP;
    ret.end=MIN_TIMESTAMP;
    if (ve == NULL) {
        return interval; 
    }
    if (ve->isEmpty() || (ve->versions.getFirstTimestamp() > interval.end())) {
        if (getMaxReadMark(key) > interval.end) {
            return ret;
        }
        
        Timestamp start = max(interval.start, getMaxReadMark(key));
        ret.start=start;
        ret.end=end;
        return ret;
    }

    Version* selected_version = NULL;   
    Version* selected_pending = NULL;

    OrderedSetNode* prev = NULL;
    OrderedSetNode* node;

    node = ve->versions.find(interval.start, prev);

    if (node->timestamp != interval.start) {
        if (prev->getVersion() !=  NULL) {
            node = prev;
        }
    }
   //         --it; //TODO: check I am not doing -- on the first element of the set; if it is the first element of the set, I need to compare with the read mark;
   //     } 
        //else {
            //if (getMaxReadMark(key) > interval.end) {
                //Version* dummy = new Version(getMaxReadMark(key), NO_SUCH_VERSION);
                //return dummy;
                
            //}
            //if (dummy->timestamp > interval.start) {
                //interval.start = dummy->timestamp; //TODO is this necessary? do I have to change anything if this happens?
            //}
        //}
    }

    OrderedSetNode* next = node;
    Version * ver = node->getVersion();

    Timestamp next_timestamp = 0;
    Timestamp next_timestamp_pending = 0;

    while ((ver != NULL) && (ver->timestamp <= interval.end)) { //TODO + duration
        next = node->getNext();
        if (ver->state == COMMITTED) {
            if ((selected_version == NULL) && (ver->maxReadFrom < interval.end)){
                selected_version =  ver;
                next_timestamp = next->getTimestamp();
            } else {
                if (getIntersection(ver->maxReadFrom, next->getTimestamp(), interval.start, interval.end) > getIntersection(selected_version->maxReadFrom, next_timestamp, interval.start, interval.end)) {
                    selected_version = ver;
                    next_timestamp = next->getTimestamp();
                }
            }
        } else if (ver->state == PENDING) {
            if ((selected_pending == NULL) && ((ver->timestamp + ver->duration) < interval.end)){
                selected_pending = ver;
                next_timestamp_pending = next->getTimestamp();
            } else {
                if (getIntersection(ver->timestamp + ver->duration, next->getTimestamp(), interval.start, interval.end) > getIntersection(selected_pending->timestamp + selected_pending->duration, next_timestamp_pending, interval.start, interval.end)) {
                    selected_pending=ver;
                    next_timestamp_pending = next->getTimestamp();
                }
            }
        }
        node = node->getNext();
        ver = node->vetVersion();
    }
    if (selected_version != NULL) {
        Timestamp start = max(interval.start, selected_version->maxReadFrom);
        Timestamp end = min(interval.end, next_timestamp);
        ret.start = start;
        ret.end = end;
        return ret;
    }
    if (selected_pending != NULL) {
        Timestamp start = max(interval.start, selected_pending->start + selected_pending->duration);
        Timestamp end = min(interval.end, next_timestamp_pending);
        ret.start = start;
        ret.end = end;
        return ret;
    }
    return ret;

}

Timespan getIntersection(Timestamp ts1Left, Timestamp ts1Right, Timestamp ts2Left, Timestamp ts2Right) {
    //TODO implement this
    return 0;
}


void VersionManager::removeVersion(Key k, Version* v) {
   //TODO 
    
}
void VersionManager::tryReadWriteLock(Key k, TimestampInterval interval, LockInfo* lockInfo) {

}


// VersionManagerEntry methods
VersionManagerEntry::VersionManagerEntry(Key k) : key(k),readMark(MIN_TIMESTAMP) {
}

VersionManagerEntry::VersionManagerEntry(Key k, Version* v) : key(k), readMark(MIN_TIMESTAMP),  {
    versions.insert(v->timestamp, v);
}

VersionManagerEntry::VersionManagerEntry(Key k, Timestamp readM) : key(k), readMark(readM) {
}

VersionManagerEntry::~VersionManagerEntry() {
}

inline bool VersionManagerEntry::VersionManagerEntry::isEmpty() {
    if (versions.size() == 0) return true;
    return false;
}


void VersionManagerEntry::purgeVersions(Timestamp barrier) {
    //TODO remove all versions with timestamps smaller than the barrier form the versionManagerEntry; then go and do the same thing in the dataStore
}

inline void VersionManagerEntry::lockEntry() {
    lock.lock();
}

inline void VersionManagerEntry::unlockEntry() {
    lock.unlock();
}
