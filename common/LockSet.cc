#include "lockSet.h"

LockSet::lock(Key k) {
   size_t index = hashKey(k);
   theLocks.at(index).lock();
}

LockSet::unlock(Key k) {
    size_t index = hashKey(k);
    theLocks.at(index).unlock();
}

size_t hashKey(Key k) {
   return (hasher((std::string)k)%NUM_LOCKS); 
}
