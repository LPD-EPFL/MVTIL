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

#ifndef _TIMER_H_
#define _TIMER_H_

#include "getticks.h"

#define DEFAULT_DURATION 10000000

class Timer {
    public:
        Timer() : duration(DEFAULT_DURATION) {}

        Timer(ticks d) : duration(d) {
        }

        inline void start() {
            begin = getticks();
        }

        inline void setTimeout(ticks d){
            duration = d; 
        }

        inline bool timeout() {
            ticks now = getticks();
            if ((begin + duration) < now) {
                return true;
            }
            return false;
        }

    private:
        ticks begin;
        ticks duration;
};

#endif
