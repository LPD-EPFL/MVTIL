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

#ifndef _LOCK_SET_H_
#define _LOCK_SET_H_

#include <mutex>
#include <array>
#include <functional>
#include <string>
#include <thread>
#include "common.h"

#define NUM_LOCKS 32

class LockSet {
    public:
        void lock(Key k);
        void unlock(Key k);
    private:
        std::array<std::recursive_mutex, NUM_LOCKS> theLocks;
        std::hash<std::string> hasher;
        size_t hashKey(Key k);
        std::thread::id threadId;
};

#endif

