#include "LockSet.h"

void LockSet::lock(Key k) {
   size_t index = hashKey(k);
   theLocks.at(index).lock();
}

void LockSet::unlock(Key k) {
    size_t index = hashKey(k);
    theLocks.at(index).unlock();
}

size_t LockSet::hashKey(Key k) {
   return (hasher((std::string)k)%NUM_LOCKS); 
}
