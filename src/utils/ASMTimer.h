/*
 * ASMTimer.h
 *
 *  Created on: 2013-12-26
 *      Author: Fanjing
 */

#ifndef _ASMTIMER_H_
#define _ASMTIMER_H_

#include <windows.h>
#include <iostream>

class ASMTimer {
private:
    // singleton mode
    int initRound;
    static ASMTimer* ptr_singleton;
    ASMTimer();  // default constructor, perform initializations
    virtual ~ASMTimer();

protected:
    bool initialized; // if the hardware supports high-resolution performance counter
    bool isTiming;    // if the timer is running currently
    bool gotTime;     // if the timer has stopped and successfully got the time

    ULARGE_INTEGER frequency; // the current performance-counter frequency, in counts per second
    INT64 ecc;
    ULARGE_INTEGER counter_start; // value of the performance counter at the moment when start() is called
    ULARGE_INTEGER counter_end; // value of the performance counter at the moment when end() is called
public:
    static ASMTimer * request();
    static void release();

    const bool available() const; // test if the very PreciseTimer instance can be used
    void start();  // start timing
    void end();    // stop timing

    const UINT64 getCounts() const; //get the time interval in counts
    const UINT64 getMilliseconds() const; // get the time interval length in milliseconds
    const UINT64 getFrequency() const;
    const INT64 getECC() const;
};

inline ASMTimer::ASMTimer() {
    unsigned long startHigh = 0;
    unsigned long startLow = 0;
    unsigned long endHigh = 0;
    unsigned long endLow = 0;
    initialized = true;

    isTiming = false;
    gotTime = false;
    initRound = 10;
    ULARGE_INTEGER t = 0; // 用于消除误差
    for (int i = 0; i < initRound; ++i) {
        start();
        end();
        t = getCounts();
        ecc = t < ecc ? t : ecc;
    }
    start();
    Sleep(1000);
    end();
    counter_start.HighPart = startHigh;
    counter_start.LowPart = startLow;
    counter_end.HighPart = endHigh;
    counter_end.LowPart = endLow;
    frequency.QuadPart = counter_end.QuadPart - counter_start.QuadPart;
    initialized = frequency.QuadPart > 0 ? true : false;
    gotTime = false;
    if (!initialized) {
        std::cout << "initialize asm timer failed" << std::endl;
    }
}

inline void ASMTimer::start() {
    if (initialized && !isTiming) {
        isTiming = true;
        gotTime = false;
        unsigned long startHigh = 0;
        unsigned long startLow = 0;

        __asm__ __volatile__(
                "RDTSC              ;"
                "mov    %0, %%edx   ;"
                "mov    %1, %%eax   ;"
                :"=r"(startHigh),"=r"(startLow)
                :
                : "%eax","%edx"
        );
        counter_start.HighPart = startHigh;
        counter_start.LowPart = startLow;
    } else {
        std::cout << "start asm timer failed" << std::endl;
    }
}

inline void ASMTimer::end() {
    if (isTiming) {
        unsigned long endHigh = 0;
        unsigned long endLow = 0;

        __asm__ __volatile__(
                "RDTSC              ;"
                "mov    %0, %%edx   ;"
                "mov    %1, %%eax   ;"
                :"=r"(endHigh),"=r"(endLow)
                :
                : "%eax","%edx"
        );
        counter_end.HighPart = endHigh;
        counter_end.LowPart = endLow;
        isTiming = false;
        gotTime = true;
    } else {
        std::cout << "end asm timer failed" << std::endl;
    }
}



#endif /* _ASMTIMER_H_ */
