// PreciseTimer.h
// This header file defines a class named PreciseTimer.
// PreciseTimer helps compute time intevals between tow events.
// Tested on Windows XP and compiled by
//   * Microsoft VC++ 6.0
//   * Borland C++Builder 6
//   * MinGW GCC 3.4.5
// Author: 龙第九子
// Written on May 12, 2006
// add ecTime by Fanjing 1312081829

#ifndef PRECISE_TIMER_H
#define PRECISE_TIMER_H

#include <windows.h>

class PreciseTimer
{
public:
    PreciseTimer();  // default constructor, perform initializations

    bool available();  // test if the very PreciseTimer instance can be used
    void start();  // start timing
    void end();    // stop timing

    long getCounts();	//get the time interval in counts.
    long getMicroseconds();  // get the time interval length in microseconds
    long getMilliseconds();  // get the time interval length in milliseconds
    long getSeconds();       // get the time interval length in seconds
    long getCountsEC();        //get the time interval in counts.
    long getMicrosecondsEC();  // get the time interval length in microseconds
    long getMillisecondsEC();  // get the time interval length in milliseconds

protected:
    bool initialized; // if the hardware supports high-resolution performance counter
    bool isTiming;    // if the timer is running currently
    bool gotTime;     // if the timer has stopped and successfully got the time

    long ecTime;	// the error correction time.
    LARGE_INTEGER frequency;  // the current performance-counter frequency, in counts per second
    LARGE_INTEGER counter_start; // value of the performance counter at the moment when start() is called
    LARGE_INTEGER counter_end; // value of the performance counter at the moment when end() is called
};

PreciseTimer::PreciseTimer()
{
    initialized = QueryPerformanceFrequency(&frequency) ? true : false;
    isTiming    = false;
    gotTime     = false;

    counter_start.QuadPart = 0;
    counter_end.QuadPart   = 0;

    this->start();
    this->end();
    ecTime = getCounts();
}

bool PreciseTimer::available()
{
    return initialized;
}

void PreciseTimer::start()
{
    if(initialized && !isTiming)
    {
        isTiming = true;
        gotTime  = false;
        QueryPerformanceCounter(&counter_start);
    }
}

void PreciseTimer::end()
{
    if(isTiming)
    {
        QueryPerformanceCounter(&counter_end);
        isTiming = false;
        gotTime  = true;
    }
}

long PreciseTimer::getCountsEC() {
	if (gotTime) {
		return (counter_end.QuadPart - counter_start.QuadPart-ecTime);
	} else {
		return 0;
	}
}

inline long PreciseTimer::getMicroseconds() {
    if(gotTime && frequency.QuadPart)
        return (counter_end.QuadPart - counter_start.QuadPart) * 1000000 / frequency.QuadPart;
    else
        return 0;
}

inline long PreciseTimer::getMicrosecondsEC() {
    if(gotTime && frequency.QuadPart)
        return (counter_end.QuadPart - counter_start.QuadPart-ecTime) * 1000000 / frequency.QuadPart;
    else
        return 0;
}

long PreciseTimer::getMillisecondsEC()
{
    if(gotTime && frequency.QuadPart)
        return (counter_end.QuadPart - counter_start.QuadPart-ecTime) * 1000 / frequency.QuadPart;
    else
        return 0;
}

long PreciseTimer::getCounts() {
	if (gotTime) {
		return (counter_end.QuadPart - counter_start.QuadPart);
	} else {
		return 0;
	}
}

long PreciseTimer::getMilliseconds()
{
    if(gotTime && frequency.QuadPart)
        return (counter_end.QuadPart - counter_start.QuadPart) * 1000 / frequency.QuadPart;
    else
        return 0;
}

long PreciseTimer::getSeconds()
{
    return getMilliseconds() / 1000;
}

#endif
