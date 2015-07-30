#ifndef _TIMER_H_
#define _TIMER_H_

#include "getticks.h"

class Timer {
    public:
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
