/* 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "VersionManager.h"

VersionManager::VersionManager() {
}

#ifndef INITIAL_TESTING
VersionManager::VersionManager(Log& l) {
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


bool VersionManager::updateAndPersistVersion(Key key, Version* v, Timestamp new_ts, Timestamp new_duration, Timestamp newPotentialReadMax, OpState new_state) {

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

bool VersionManager::persistRemoveVersion(Key k, Timestamp t) {
    //TODO: remove version from RocksDB
    return true;
}

void VersionManager::tryReadLock(Key key, TimestampInterval interval, LockInfo& lockInfo) {
    bool pending_exists=false;
    VersionManagerEntry* ve = getVersionSet(key);
    if (ve == NULL) {
        markReadNotFound(key, interval.finish); //be conservative
        lockInfo.state = OperationState::FAIL_NO_VERSION;
        return;
    }
    ve->lockEntry();
    Version* selected_version = NULL;
    Timestamp next_timestamp = 0;

    OrderedSetNode* prev = NULL;
    OrderedSetNode* node;
    node = ve->versions.find(interval.start, &prev);

    if (node->getTimestamp() != interval.start) {
        if (prev->getVersion() != NULL) {
            node = prev;
        }
    }
   
    OrderedSetNode* next= node;
    Version *ver = node->getVersion();

    while ((ver != NULL) && (ver->timestamp <= interval.finish)) {
        next = node->getNext();
        if (ver->state == COMMITTED) {
            if (selected_version == NULL) {
                selected_version = ver;
                next_timestamp = next->getTimestamp();
            } else {
                if (getIntersection(ver->timestamp,next->getTimestamp(),interval.start,interval.finish) > getIntersection(selected_version->timestamp, next_timestamp, interval.start, interval.finish)) {
                    selected_version = ver;
                    next_timestamp = next->getTimestamp();
                }
            }
        } else if (ver->state == PENDING) {
            pending_exists = true;
        }
        node=node->getNext();
        ver = node->getVersion();
    }
    if (selected_version == NULL) {
        markReadNotFound(key, interval.finish);
        if (pending_exists == true) {
            lockInfo.state = OperationStats::FAIL_PENDING_VERSION;
        } else {
            lockInfo.state = OperationState::FAIL_NO_VERSION;
        }
        ve->unlockEntry();
        return;
    }


    lockInfo.locked.start = selected_version->timestamp;
    lockInfo.locked.finish = std::min(next_timestamp, interval.finish);
    lockInfo.potential.start = selected_version->timestamp;
    lockInfo.potential.finish = next_timestamp;
    lockInfo.version = selected_version;
    lockInfo.state = OperationState::R_LOCK_SUCCESS;

    if (selected_version->maxReadFrom < lockInfo.locked.finish) {
        selected_version->maxReadFrom = lockInfo.locked.finish;
    }
    
    ve->unlockEntry();
    return;

}

void VersionManager::tryWriteLock(Key key, TimestampInterval interval, LockInfo& lockInfo) {
    VersionManagerEntry* ve = getVersionSet(key);
    if (ve == NULL) {
        ve = createNewEntry(key);
    }
    ve->lockEntry();
    if (getMaxReadMark(key) > interval.finish) {
        lockInfo.state = OperationState::FAIL_READ_MARK_LARGE;    
        lockInfo.potential.start = getMaxReadMark(key);
        ve->unlockEntry();
        return;
    }
    if (ve->isEmpty() || (ve->versions.getFirstTimestamp() > interval.finish)) {
        if (getMaxReadMark(key) > interval.finish) {
            lockInfo.state = OperationState::FAIL_READ_MARK_LARGE;    
            lockInfo.potential.start = getMaxReadMark(key);
            ve->unlockEntry();
            return;
        }
        
        Timestamp start = std::max(interval.start, getMaxReadMark(key));
        if (ve->isEmpty()) {
            lockInfo.potential.finish = MAX_TIMESTAMP;
        } else {
            lockInfo.potential.finish = ve->versions.getFirstTimestamp();
        }
        Version* new_version = new Version(start, interval.finish - start, PENDING, start); 
        ve->versions.insert(start, new_version); 
        lockInfo.locked.start = start;
        lockInfo.locked.finish = interval.finish;
        lockInfo.potential.start = getMaxReadMark(key); 
        lockInfo.version = new_version;
        lockInfo.state = OperationState::W_LOCK_SUCCESS;
        ve->unlockEntry();
        return;
    }

    Version* selected_version = NULL;   
    Version* selected_pending = NULL;

    OrderedSetNode* node;
    OrderedSetNode* prev = NULL;
    node = ve->versions.find(interval.start, &prev);

    if (node->getTimestamp() != interval.start) {
        if (prev->getVersion() != NULL) {
            node = prev;
        } else {
            //TODO: this should never happen according to what I saw earlier in this method
        }
    }

    TimestampInterval candidate;
    candidate.start = MIN_TIMESTAMP;
    candidate.finish = MIN_TIMESTAMP;
    TimestampInterval candidate_pending;
    candidate_pending.start = MIN_TIMESTAMP;
    candidate_pending.finish = MIN_TIMESTAMP;
 

    OrderedSetNode* next = node;
    Version* ver = node->getVersion();

    Timestamp next_timestamp = 0;
    Timestamp next_timestamp_pending = 0;

    while ((ver != NULL) && (node->getTimestamp() <= interval.finish)) {
        next = node->getNext();
        if (ver->state == COMMITTED) {
            if ((next->getTimestamp() - ver->maxReadFrom) > (candidate.finish - candidate.start)) {
                candidate.start = ver->maxReadFrom;
                candidate.finish = next->getTimestamp();
            }
            if ((selected_version == NULL) && (ver->maxReadFrom < interval.finish)){
                selected_version =  ver;
                next_timestamp = next->getTimestamp();
            } else {
                if (getIntersection(ver->maxReadFrom, next->getTimestamp(), interval.start, interval.finish) > getIntersection(selected_version->maxReadFrom, next_timestamp, interval.start, interval.finish)) {
                    selected_version = ver;
                    next_timestamp = next->getTimestamp();
                }
            }
        } else if (ver->state == PENDING) {
            if ((next->getTimestamp() - (ver->timestamp + ver->duration)) > (candidate_pending.finish - candidate_pending.start)) {
               candidate_pending.start = ver->timestamp + ver->duration;  
               candidate_pending.finish = next->getTimestamp();
            }
            if ((selected_pending == NULL) && ((ver->timestamp + ver->duration) < interval.finish)){
                selected_pending = ver;
                next_timestamp_pending = next->getTimestamp();
            } else {
                if (getIntersection(ver->timestamp + ver->duration, next->getTimestamp(), interval.start, interval.finish) > getIntersection(selected_pending->timestamp + selected_pending->duration, next_timestamp_pending, interval.start, interval.finish)) {
                    selected_pending = ver;
                    next_timestamp_pending = next->getTimestamp();
                }
            }
        }
        node = node->getNext();
        ver = node->getVersion();
    }
    if (selected_version != NULL) {
        Timestamp start = std::max(interval.start, selected_version->maxReadFrom);
        Timestamp end = std::min(interval.finish, next_timestamp);
        Version* new_version = new Version(start, end - start, PENDING, start);
        ve->versions.insert(start, new_version); 
        lockInfo.version = new_version;
        lockInfo.locked.start = start;
        lockInfo.locked.finish = end;
        lockInfo.potential.start = selected_version->maxReadFrom;
        lockInfo.potential.finish = next_timestamp;
        lockInfo.state = OperationState::W_LOCK_SUCCESS;
        ve->unlockEntry();
        return; 
    }
    if (selected_pending != NULL) {
        Timestamp start = std::max(interval.start, selected_pending->timestamp + selected_pending->duration);
        Timestamp end = std::min(interval.finish, next_timestamp_pending);
        Version* new_version = new Version(start, end - start, PENDING, start);
        ve->versions.insert(start, new_version); 
        lockInfo.version = new_version;
        lockInfo.locked.start = start;
        lockInfo.locked.finish = end;
        lockInfo.potential.start = selected_pending->timestamp + selected_pending->duration;
        lockInfo.potential.finish = next_timestamp_pending;
        lockInfo.state = OperationState::W_LOCK_SUCCESS;
        ve->unlockEntry();
        return;
    }
    lockInfo.state = OperationState::FAIL_INTERSECTION_EMPTY;
    if (candidate.start != MIN_TIMESTAMP) {
        lockInfo.potential.start = candidate.start;
        lockInfo.potential.finish = candidate.finish;
    } else if (candidate_pending.start != MIN_TIMESTAMP) {
        lockInfo.potential.start = candidate_pending.start;
        lockInfo.potential.finish = candidate_pending.finish;
    } else {
        lockInfo.potential.start = MIN_TIMESTAMP;
        lockInfo.potential.finish = MIN_TIMESTAMP;
    }
    ve->unlockEntry();
    return;

}

void VersionManager::getWriteLockHint(Key key, TimestampInterval interval, LockInfo& lockInfo) {
    VersionManagerEntry* ve = getVersionSet(key);
    TimestampInterval ret;
    ret.start=MIN_TIMESTAMP;
    ret.finish=MIN_TIMESTAMP;
    if (ve == NULL) {
        return interval; 
    }
    if (getMaxReadMark(key) > interval.finish) {
        return ret;
    }
    if (ve->isEmpty() || (ve->versions.getFirstTimestamp() > interval.finish)) {
        if (getMaxReadMark(key) > interval.finish) {
            return ret;
        }
        
        Timestamp start = std::max(interval.start, getMaxReadMark(key));
        ret.start=start;
        ret.finish=interval.finish;
        return ret;
    }

    Version* selected_version = NULL;   
    Version* selected_pending = NULL;

    OrderedSetNode* prev = NULL;
    OrderedSetNode* node;

    node = ve->versions.find(interval.start, &prev);

    if (node->getTimestamp() != interval.start) {
        if (prev->getVersion() !=  NULL) {
            node = prev;
        } else {
            //TODO this should never happen; handle the case when it does
        }
    }

    OrderedSetNode* next = node;
    Version * ver = node->getVersion();

    Timestamp next_timestamp = 0;
    Timestamp next_timestamp_pending = 0;

    while ((ver != NULL) && (ver->timestamp <= interval.finish)) {
        next = node->getNext();
        if (ver->state == COMMITTED) {
            if ((selected_version == NULL) && (ver->maxReadFrom < interval.finish)){
                selected_version =  ver;
                next_timestamp = next->getTimestamp();
            } else {
                if (getIntersection(ver->maxReadFrom, next->getTimestamp(), interval.start, interval.finish) > getIntersection(selected_version->maxReadFrom, next_timestamp, interval.start, interval.finish)) {
                    selected_version = ver;
                    next_timestamp = next->getTimestamp();
                }
            }
        } else if (ver->state == PENDING) {
            if ((selected_pending == NULL) && ((ver->timestamp + ver->duration) < interval.finish)){
                selected_pending = ver;
                next_timestamp_pending = next->getTimestamp();
            } else {
                if (getIntersection(ver->timestamp + ver->duration, next->getTimestamp(), interval.start, interval.finish) > getIntersection(selected_pending->timestamp + selected_pending->duration, next_timestamp_pending, interval.start, interval.finish)) {
                    selected_pending=ver;
                    next_timestamp_pending = next->getTimestamp();
                }
            }
        }
        node = node->getNext();
        ver = node->getVersion();
    }
    if (selected_version != NULL) {
        Timestamp start = std::max(interval.start, selected_version->maxReadFrom);
        Timestamp end = std::min(interval.finish, next_timestamp);
        ret.start = start;
        ret.finish = end;
        return ret;
    }
    if (selected_pending != NULL) {
        Timestamp start = std::max(interval.start, selected_pending->timestamp + selected_pending->duration);
        Timestamp end = std::min(interval.finish, next_timestamp_pending);
        ret.start = start;
        ret.finish = end;
        return ret;
    }
    return ret;

}

void VersionManager::tryExpandRead(Key k, Timestamp versionTimestamp, TimestampInterval newInterval, LockInfo& lockInfo) {

}

void VersionManager::tryExpandWrite(Key k, Timestamp versionTimestamp, TimestampInterval newInterval, LockInfo& lockInfo) {

}

Timespan VersionManager::getIntersection(Timestamp ts1Left, Timestamp ts1Right, Timestamp ts2Left, Timestamp ts2Right) {
    if (ts1Right < ts1Left) return 0;
    if (ts2Right < ts2Left) return 0;
    Timestamp l = std::max(ts1Left, ts2Left);
    Timestamp r = std::min(ts1Right, ts2Right);
    if (r < l) return 0;
    Timespan d = r - l;
    return d;
}


int VersionManager::removeVersion(Key k, Version* v) {
    VersionManagerEntry* ve = getVersionSet(k);
    if (ve == NULL) {
        return 0;
    }
    int res = ve->versions.remove(v->timestamp);
    return res;
}

void VersionManager::tryReadWriteLock(Key k, TimestampInterval interval, LockInfo& lockInfo) {
    //TODO
}


// VersionManagerEntry methods
VersionManager::VersionManagerEntry::VersionManagerEntry(Key k) : key(k),readMark(MIN_TIMESTAMP) {}

VersionManager::VersionManagerEntry::VersionManagerEntry(Key k, Version* v) : key(k), readMark(MIN_TIMESTAMP)  {
    versions.insert(v->timestamp, v);
}

VersionManager::VersionManagerEntry::VersionManagerEntry(Key k, Timestamp readM) : key(k), readMark(readM) {}

VersionManager::VersionManagerEntry::~VersionManagerEntry() {
} 


void VersionManager::VersionManagerEntry::collectTo(Timestamp barrier, std::queue<Timestamp>* q) {
    lock.lock();
    Timestamp ts = versions.removeTo(barrier, q);
    if (ts < barrier) { //this should never happen
        ts = barrier;
    }
    if (ts > readMark) {
        readMark = ts;
    }
    lock.unlock();
}


