/*   
 *   File: LockManager.cpp
 *   Author: Junxiong Wang <junxiong.wang@epfl.ch>
 *   Description: 
 *   LockManager.cpp is part of MVTLSYS
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

#include "LockManager.h"

LockManager::LockManager(Key k) : key(k){
    head = new IntervalLock;
    head->top_level = MAX_LEVEL;
    head->interval.start = MIN_TIMESTAMP;
    head->interval.finish = MIN_TIMESTAMP;
    head->lock_operation = LockOperation::WRITE;
    head->transaction_id = 0;
    head->is_committed = true;
    IntervalLock *tail = new IntervalLock;
    tail->top_level = 1;
    tail->interval.start = MAX_TIMESTAMP;
    tail->interval.finish = MAX_TIMESTAMP;
    tail->lock_operation = LockOperation::WRITE;
    tail->transaction_id = 0;
    tail->is_committed = true;
    for(int i=0;i<MAX_LEVEL;i++){
        head->next[i] = tail;
        tail->next[i] = NULL;
    }
}

bool LockManager::LockReadInterval(TransactionId tid, TimestampInterval& candidate_interval){
    #ifdef DEBUG
        std::cout<<"LockManager: Read interval["<<candidate_interval.lock_start<<","<<candidate_interval.finish<<"]"<<endl;
    #endif
    Timestamp searchTimestamp = candidate_interval.lock_start;
    IntervalLock* node = head;
    for(int level=node->top_level-1; level >=0; level--) {
        while (node->next[level] != NULL && (node->next[level]->interval).start <= searchTimestamp ) {
            if((node->next[level]->interval.finish >= searchTimestamp) && TestConflict(node->next[level]->lock_operation,LockOperation::READ)){
               #ifdef DEBUG
                  std::cout<<"Read conflict happens!"<<endl;
               #endif
               return false;
            }
            node = node->next[level];
        }
    }

    IntervalLock* prev = node;
    IntervalLock* next = node->next[0];

    //One policy
    candidate_interval.finish = min(candidate_interval.finish, next->interval.start - 1);
    if(candidate_interval.start > candidate_interval.finish){
        return false;
    }

    TimestampInterval lock_interval;
    lock_interval.start = candidate_interval.lock_start;
    lock_interval.finish = candidate_interval.finish;

    IntervalLock* lock = CreateReadLock(lock_interval);
    lock->transaction_id = tid;
    for(int i=0;i<prev->top_level;i++)
        prev->next[i] = lock;
    for(int i=0;i<lock->top_level;i++)
        lock->next[i] = next;
    return true;
}

bool LockManager::LockWriteInterval(TransactionId tid, TimestampInterval& candidate_interval){
    #ifdef DEBUG
        std::cout<<"LockManager: Write interval["<<candidate_interval.start<<","<<candidate_interval.finish<<"]"<<endl;
    #endif

    Timestamp searchTimestamp = candidate_interval.finish;
    IntervalLock* node = head;
    int level = 0;
    while (node->next[level] != NULL && (node->next[level]->interval).start <= searchTimestamp){
        if(node->next[level]->transaction_id != tid && TestConflict(node->next[level]->lock_operation, LockOperation::WRITE))
        {
            const TimestampInterval& conflictInterval = node->next[level]->interval;
            if(intersects(candidate_interval, conflictInterval)){
                if(candidate_interval.start >= conflictInterval.start && candidate_interval.finish <= conflictInterval.finish){
                    #ifdef DEBUG
                        std::cout<<"Write1 conflict happens!tid:"<<node->next[level]->transaction_id<<",Interval:"<<conflictInterval.start<<","<<conflictInterval.finish <<endl;
                    #endif
                    return false; 
                }
                else if(candidate_interval.start >= conflictInterval.start){
                    candidate_interval.start = conflictInterval.finish + 1;
                }
                else if(candidate_interval.finish <= conflictInterval.finish){
                    candidate_interval.finish = conflictInterval.start - 1;
                }
            }
        }
        node = node->next[level];
    }

    if(candidate_interval.start > candidate_interval.finish){
        #ifdef DEBUG
            std::cout<<"Write2 conflict happens!"<<endl;
        #endif
        return false;
    }

    node = head;
    IntervalLock* lock = CreateWriteLock(candidate_interval);
    lock->transaction_id = tid;
    for(int level=node->top_level-1; level >=0; level--) {
        while (node->next[level] != NULL && (node->next[level]->interval).start <= candidate_interval.start){
            node = node->next[level];
        }
    }
    IntervalLock* next = node->next[0];
    for(int i=0;i<node->top_level;i++)
        node->next[i] = lock;
    for(int i=0;i<lock->top_level;i++)
        lock->next[i] = next;
    return true;
}

void LockManager::CommitInterval(TransactionId tid, const Timestamp& old_ts,const Timestamp& commit_ts){
    IntervalLock* node = head;
    for(int level=node->top_level-1; level >=0; level--) {
        while (node->next[level] != NULL && (node->next[level]->interval).start < old_ts ) {
            node = node->next[level];
        }
    }
    node = node->next[0];
    
    while(node != NULL && node->interval.start == old_ts)
    {
        if(node->transaction_id == tid){
            node->is_committed = true;
            if(node->lock_operation == LockOperation::WRITE)
            {
                #ifdef DEBUG
                    std::cout<<"LockManager: Modify Write Lock interval["<<commit_ts<<","<<commit_ts<<"]"<<std::endl;
                #endif
                node->interval.start = commit_ts;
                node->interval.finish = commit_ts;
            }
            else
            {
                #ifdef DEBUG
                    std::cout<<"LockManager: Modify Read Lock interval to ["<<commit_ts<<"]"<<std::endl;
                #endif
                node->interval.finish = commit_ts;
            }
        }
        node = node->next[0];
    }

    #ifdef DEBUG
        node = head;
        while (node->next[0] != NULL) {
            std::cout<<"["<<node->interval.start<<","<<node->interval.finish<<"]"<<" ";          
            node = node->next[0];
        }
        std::cout<<std::endl;
    #endif
}


IntervalLock* LockManager::CreateReadLock(TimestampInterval read_interval){
    #ifdef DEBUG
        std::cout<<"LockManager: Create Read Lock interval["<<read_interval.start<<","<<read_interval.finish<<"]"<<std::endl;
    #endif
    IntervalLock *lock = new IntervalLock;
    lock->interval = read_interval;
    lock->lock_operation = LockOperation::READ;
    lock->is_committed = false;
    lock->top_level = GetRandomLevel();
    for(int i=0;i<MAX_LEVEL;i++)
        lock->next[i] = NULL;
    size++;
    return lock;
}

IntervalLock* LockManager::CreateWriteLock(TimestampInterval write_interval){
    #ifdef DEBUG
        std::cout<<"LockManager: Create Write Lock interval["<<write_interval.start<<","<<write_interval.finish<<"]"<<endl;
    #endif
    IntervalLock *lock = new IntervalLock;
    lock->interval = write_interval;
    lock->lock_operation = LockOperation::WRITE;
    lock->is_committed = false;
    lock->top_level = GetRandomLevel();
    for(int i=0;i<MAX_LEVEL;i++)
        lock->next[i] = NULL;
    size++;
    return lock;
}

bool LockManager::RemoveLock(TransactionId tid, const Timestamp& old_ts){
    #ifdef DEBUG
        std::cout<<"LockManager: Remove Lock, Timestamp:"<<old_ts<<endl;
    #endif

    IntervalLock* node = head;
    for(int level=node->top_level-1; level >=0; level--) {
        while (node->next[level] != NULL && (node->next[level]->interval).start < old_ts ) {
            node = node->next[level];
        }
    }

    #ifdef DEBUG
        std::cout<<node->interval.start<<node->interval.finish<<endl;
    #endif
    while (node->next[0] != NULL && (node->next[0]->interval).start == old_ts) {
        if(node->next[0]->transaction_id == tid){
            IntervalLock* nextNode = node->next[0];
            for(int i=0;i<node->top_level;i++){
                node->next[i] = nextNode->next[0];
            }
            delete nextNode;
            size--;             
        }
        else{
            node = node->next[0];
        }
    }
    return true;
}

bool LockManager::GarbageCollection(Timestamp ts){
    IntervalLock* node = head->next[0];
    while (node->next[0] != NULL && (node->next[0]->interval).start <= ts) {
        if((node->next[0]->interval).finish <= ts){
            IntervalLock* cur = node->next[0];
            for(int i=0;i<node->top_level;i++){
                node->next[i] = cur->next[0];
            }
            delete cur;
            size--;
        }
        node = node->next[0];
    }
    return true;
}
