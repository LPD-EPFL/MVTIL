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

#include "LockSet.h"

void LockSet::lock(Key k) {
   size_t index = 0;
   theLocks.at(index).lock();
   threadId = std::this_thread::get_id();
}

void LockSet::unlock(Key k) {
    size_t index = 0;
    theLocks.at(index).unlock();
}

size_t LockSet::hashKey(Key k) {
   return (hasher((std::string)k)%NUM_LOCKS); 
}
