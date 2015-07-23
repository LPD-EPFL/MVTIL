#include "VersionManager.h"

VersionManager::VersionManager() {
}

VersionManager::VersionManager(Log* l) {
    //TODO: create the versions data structure from the log
}

VersionManager::~VersionManager() {
}

Version* VersionManager::getVersion(Key k, TimestampInterval interval, op_type flag) {
    if (flag == OP_READ) {
        VersionManagerEntry* ve = versionStore.get(key); //TODO replace with the actual function for this
        if (ve == NULL) {
            markReadNotFound(key, interval.end); //be conservative
            return NULL;
        }
        Version* selected_version = NULL;
        Timestamp next_timestamp = 0;
        std::set<Version>::iterator it; 
        it = versions.lower_bound(interval.start); //Points to the first entry to be considered
        if (it->timestamp != interval.start) {
            if (it != versions.start()) {
                --it; //TODO: check I am not doing -- on the first element of the set;
            }
        }
        std::set<Version>::iterator it_next = it;
        while ((it != versions.end()) && (it->timestamp <= interval.end)) {
            ++it_next;
            if (it->state == COMMITTED) {
                if (selected_version == NULL) {
                    selected_version = it;
                    next_timestamp = it_next->timestamp;
                } else {
                    if (getIntersection(it->timestamp,it_next->timestamp,interval.start,interval.end) > getIntersection(selected_version->timestamp, next_timestamp, interval.start, interval.end)) {
                        selected_version = it;
                        next_timestamp = it_next->timestamp;
                    }
                }
            }
            ++it;
        }
        if (selected_version == NULL) {
            markReadNotFound(key, interval.end);
            return NULL;
        }
        return selected_version;
    } else if (flag == OP_WRITE) {
        VersionManagerEntry* ve = versionStore.get(key); //TODO replace with the actual function for this
        if ((ve == NULL) || (ve->isEmpty())) {
            Version* dummy = new Version(getMaxReadMark(key), NO_SUCH_VERSION);
            return dummy;
        }
        Version* selected_version = NULL;   
        Version* selected_pending = NULL;
        std::set<Version>::iterator it;
        it = versions.lower_bound(interval.start); //Points to the first entry to be considered
        if (it->timestamp != interval.start) {
            if (it != versions.start()) {
                --it; //TODO: check I am not doing -- on the first element of the set; if it is the first element of the set, I need to compare with the read mark;
            } else {
                if (getMaxReadMark(key) > interval.end) {
                    Version* dummy = new Version(getMaxReadMark(key), NO_SUCH_VERSION);
                    return dummy;
                }
                if (dummy->timestamp > interval.start) {
                    interval.start = dummy->timestamp; //TODO is this necessary? do I have to change anything if this happens?
                }
            }
        }

        std::set<Version>::iterator it_next = it;
        Timestamp next_timestamp = 0;
        Timestamp next_timestamp_pending = 0;
        while ((it != versions.end()) && (it->timestamp <= interval.end)) { //TODO + duration
            ++it_next;
            if (it->state == COMMITTED) {
                if (selected_version == NULL) {
                    selected_version =  it;
                    next_timestamp = it_next->timestamp;
                } else {
                    if (getIntersection(it->maxReadFrom, it_next->timestamp, interval.start, interval.end) > getIntersection(selected_version->maxReadFrom, next_timestamp, interval.start, interval.end)) {
                        selected_version = it;
                        next_timestamp = it_next->timestamp;
                    }
                }
            } else if (it->state == PENDING) {
                if (selected_pending == NULL) {
                    selected_pending = it;
                    next_timestamp_pending = it_next->timestamp;
                } else {
                    if (getIntersection(it->timestamp + it->duration, it_next->timestamp, interval.start, interval.end) > getIntersection(selected_pending->timestamp + selected_pending->duration, next_timestamp_pending, interval.start, interval.end)) {
                        selected_pending=it;
                        next_timestamp_pending = it_next->timestamp;
                    }
                }
            }
            ++it;
        }
        if (selected_version != NULL) {
            return selected_version; 
        }
        if (selected_pending != NULL) {
            return selected_pending;
        }
        Version* dummy = new Version(getMaxReadMark(key), NO_SUCH_VERSION);
        return dummy;
    }
}

void tryReadLock(Key k, TimestampInterval interval, LockInfo& lockInfo) {
    VersionManagerEntry* ve = versionStore.get(key); //TODO replace with the actual function for this
    if (ve == NULL) {
        markReadNotFound(key, interval.end); //be conservative
        //TODO any lock needed here?
        lockInfo.state = FAIL_NO_VERSION;
        return;
    }
    //TODO lock
    Version* selected_version = NULL;
    Timestamp next_timestamp = 0;
    std::set<Version>::iterator it; 
    it = versions.lower_bound(interval.start); //Points to the first entry to be considered
    if (it->timestamp != interval.start) {
        if (it != versions.start()) {
            --it; //TODO: check I am not doing -- on the first element of the set;
        }
    }
    std::set<Version>::iterator it_next = it;
    while ((it != versions.end()) && (it->timestamp <= interval.end)) {
        ++it_next;
        if (it->state == COMMITTED) {
            if (selected_version == NULL) {
                selected_version = it;
                next_timestamp = it_next->timestamp;
            } else {
                if (getIntersection(it->timestamp,it_next->timestamp,interval.start,interval.end) > getIntersection(selected_version->timestamp, next_timestamp, interval.start, interval.end)) {
                    selected_version = it;
                    next_timestamp = it_next->timestamp;
                }
            }
        }
        ++it;
    }
    if (selected_version == NULL) {
        markReadNotFound(key, interval.end);
        lockInfo.state = FAIL_NO_VERSION;
        //TODO unlock
        return;
    }


    lockInfo.locked.start = selected_version.timestamp;
    lockInfo.locked.end = min(next_timestamp, interval.end);
    lockInfo.potential.start = selected_version.timestamp;
    lockInfo.potential.end = next_timestamp;
    lockInfo.version = selected_version;
    lockInfo.state = R_LOCK_SUCCESS;

    if (selected_version.maxReadFrom < lockInfo.locked.end) {
        selected_version.maxReadFrom = lockInfo.locked.end;
    }

    //TODO: unlock
    return;

}

void tryWriteLock(Key k, TimestampInterval interval, LockInfo& lockInfo) {
    VersionManagerEntry* ve = versionStore.get(key); //TODO replace with the actual function for this
    if (ve == NULL) {
        //TODO: get lock on versionStore; create entry
    }
    //TODO lock
    if (ve->isEmpty()) {
        if (getMaxReadMark(key) > interval.end) {
            lockInfo.state = FAIL_READ_MARK_LARGE;    
            lockInfo.potential.start = getMaxReadMark(key);
            //TODO unlock   
            return;
        }
        
        Timestamp start = max(interval.start, getMaxReadMark(key));
        Version* new_version = new Version(start, interval.end, PENDING, start);
        ve->insert(new_version); 
        lockInfo.locked.start = start;
        lockInfo.locked.end = interval.end;
        lockInfo.potential.start = getMaxReadMark(key); 
        lockInfo.potential.end = MAX_TIMESTAMP;
        lockInfo.version = new_version;
        lockInfo.state = W_LOCK_SUCCESS;
        //TODO unlock
        return;
    }
    Version* selected_version = NULL;   
    Version* selected_pending = NULL;
    std::set<Version>::iterator it;
    it = versions.lower_bound(interval.start); //Points to the first entry to be considered
    if (it->timestamp != interval.start) {
        if (it != versions.start()) {
            --it; //TODO: check I am not doing -- on the first element of the set; if it is the first element of the set, I need to compare with the read mark;
        } else {
            if (getMaxReadMark(key) > interval.end) {
                Version* dummy = new Version(getMaxReadMark(key), NO_SUCH_VERSION);
                return dummy;
            }
            if (dummy->timestamp > interval.start) {
                interval.start = dummy->timestamp; //TODO is this necessary? do I have to change anything if this happens?
            }
        }
    }

    std::set<Version>::iterator it_next = it;
    Timestamp next_timestamp = 0;
    Timestamp next_timestamp_pending = 0;
    while ((it != versions.end()) && (it->timestamp <= interval.end)) { //TODO + duration
        ++it_next;
        if (it->state == COMMITTED) {
            if (selected_version == NULL) {
                selected_version =  it;
                next_timestamp = it_next->timestamp;
            } else {
                if (getIntersection(it->maxReadFrom, it_next->timestamp, interval.start, interval.end) > getIntersection(selected_version->maxReadFrom, next_timestamp, interval.start, interval.end)) {
                    selected_version = it;
                    next_timestamp = it_next->timestamp;
                }
            }
        } else if (it->state == PENDING) {
            if (selected_pending == NULL) {
                selected_pending = it;
                next_timestamp_pending = it_next->timestamp;
            } else {
                if (getIntersection(it->timestamp + it->duration, it_next->timestamp, interval.start, interval.end) > getIntersection(selected_pending->timestamp + selected_pending->duration, next_timestamp_pending, interval.start, interval.end)) {
                    selected_pending=it;
                    next_timestamp_pending = it_next->timestamp;
                }
            }
        }
        ++it;
    }
    if (selected_version != NULL) {
        return selected_version; 
    }
    if (selected_pending != NULL) {
        return selected_pending;
    }
    Version* dummy = new Version(getMaxReadMark(key), NO_SUCH_VERSION);
    return dummy;

}

void markReadNotFound(Key k, Timestamp ts) {
    std::map<Key, VersionManagerEntry*>::it = versionStore.find(k);
    if (it == versionStore.end()) {
        VersionManagerEntry* ve = new VersionManagerEntry(ts);
        versionStore.insert(std::pair<Key,VersionManagerEntry*>(k,ve));
        return;
    }
    if (it->second->readMark < ts) {
        it->second->readMark = ts;
    }
}

Timestamp getMaxReadMark(Key k) {
    std::map<Key, VersionManagerEntry*>::it = versionStore.find(k);
    if (it == versionStore.end()) {
        return MIN_TIMESTAMP;
    }
    return it->second->readMark;
}
