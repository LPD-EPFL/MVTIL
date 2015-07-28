#ifndef _LOCK_SET_H_
#define _LOCK_SET_H_

#include <mutex>
#include <array>
#include <functional>
#include <string>

#include "common.h"

#define NUM_LOCKS 32

class LockSet {
    public:
        void lock(Key k);
        void unlock(Key k);
    private:
        std::array<std::mutex, NUM_LOCKS> theLocks;
        std::hash<std::string> hasher;
        size_t hashKey(Key k);
};

#endif

