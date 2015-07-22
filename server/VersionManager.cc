#include "VersionManager.h"

VersionManager::VersionManager() {
}

VersionManager::VersionManager(Log* l) {
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
            if (selected_version == NULL) {
                selected_version = it;
                next_timestamp = it_next->timestamp;
            } else {
                if (getIntersection(it->timestamp,it_next->timestamp,interval.start,interval.end) > getIntersection(selected_version->timestamp, next_timestamp, interval.start, interval.end)) {
                    selected_version = it;
                    next_timestamp = it_next->timestamp;
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
       if (ve == NULL) {
            Version* dummy = getMaxReadMark(key);
            return dummy;
       }
       Version* selected_version = NULL;   
       std::set<Version>::iterator it;
       it = versions.lower_bound(interval.start); //Points to the first entry to be considered
       if (it->timestamp != interval.start) {
           if (it != versions.start()) {
              --it; //TODO: check I am not doing -- on the first element of the set; if it is the first element of the set, I need to compare with the read mark;
           } else {
                Version* dummy = getMaxReadMark(key);
              if (dummy->timestamp > interval.end) {
                return getMaxReadMark(key);
              }
              if (dummy->timestamp > interval.start) {
                 interval.start = dummy->timestamp; //TODO is this necessary? do I have to change anything if this happens?
              }
           }
       }
       while ((it != versions.end()) && (it->timestamp <= interval.end)) {
            if (selected_version == NULL) {
                selected_version = 
            }
       }
    }
}
