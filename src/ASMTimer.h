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
    ULARGE_INTEGER counter_ec;  // value of the error correction number.
public:
    static ASMTimer * request();
    static void release();

    bool available();  // test if the very PreciseTimer instance can be used
    void start();  // start timing
    void end();    // stop timing

    UINT64 getCountsNoEC();	//get the time interval in counts
    UINT64 getCounts(); //get the time interval in counts
    UINT64 getMilliseconds();  // get the time interval length in milliseconds
};

inline UINT64 ASMTimer::getCountsNoEC() {
    if(gotTime){
        return (counter_end.QuadPart - counter_start.QuadPart);
    }else{
        return 0;
    }
}

inline UINT64 ASMTimer::getCounts() {
    if(gotTime){
        return (counter_end.QuadPart - counter_start.QuadPart - counter_ec.QuadPart);
    }else{
        return 0;
    }
}

inline UINT64 ASMTimer::getMilliseconds() {
    if(gotTime){
        return ((counter_end.QuadPart - counter_start.QuadPart - counter_ec.QuadPart) / (frequency.QuadPart / 1000));
    }else{
        return 0;
    }
}

#endif /* _ASMTIMER_H_ */
