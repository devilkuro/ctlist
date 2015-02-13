/*
 * ASMTimer.h
 *
 *  Created on: 2013-12-26
 *      Author: Fanjing
 */

#ifndef _ASMTIMER_H_
#define _ASMTIMER_H_

#include <windows.h>

class ASMTimer {
private:
    // singleton mode
    static ASMTimer* ptr_singleton;
    ASMTimer();  // default constructor, perform initializations
    virtual ~ASMTimer();

protected:
    bool initialized; // if the hardware supports high-resolution performance counter
    bool isTiming;    // if the timer is running currently
    bool gotTime;     // if the timer has stopped and successfully got the time

    ULARGE_INTEGER frequency; // the current performance-counter frequency, in counts per second
    ULARGE_INTEGER counter_start; // value of the performance counter at the moment when start() is called
    ULARGE_INTEGER counter_end; // value of the performance counter at the moment when end() is called
public:
    static ASMTimer * request();
    static void release();

    bool available();  // test if the very PreciseTimer instance can be used
    void start();  // start timing
    void end();    // stop timing

    UINT64 getCounts(); //get the time interval in counts
    UINT64 getMilliseconds();  // get the time interval length in milliseconds
};

inline void ASMTimer::start() {
    if(initialized && !isTiming){
        isTiming = true;
        gotTime = false;
        unsigned long startHigh = 0;
        unsigned long startLow = 0;
        __asm__ __volatile__(
                ".intel_syntax noprefix\n\t"
                "RDTSC\n\t"
                "MOV        %0,     %%edx\n\t"
                "MOV        %1,      %%eax\n\t"
                ".att_syntax\n"
                :"=r"(startHigh),"=r"(startLow)
                :
                : "%eax","%edx"
        );
        counter_start.HighPart = startHigh;
        counter_start.LowPart = startLow;
    }
}

inline void ASMTimer::end() {
    if(isTiming){
        unsigned long endHigh = 0;
        unsigned long endLow = 0;
        __asm__ __volatile__(
                ".intel_syntax noprefix\n\t"
                "RDTSC\n\t"
                "MOV        %0,     %%edx\n\t"
                "MOV        %1,     %%eax\n\t"
                ".att_syntax\n"
                :"=r"(endHigh),"=r"(endLow)
                :
                : "%eax","%edx"
        );
        counter_end.HighPart = endHigh;
        counter_end.LowPart = endLow;
        isTiming = false;
        gotTime = true;
    }
}
#endif /* _ASMTIMER_H_ */
