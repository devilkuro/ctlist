/*
 * ASMTimer.h
 *
 *  Created on: 2013-12-26
 *      Author: Fanjing
 */

#ifndef _ASMTIMER_H_
#define _ASMTIMER_H_
#define ASMULONG(var) unsigned long var __asm__(""#var"") = 0
#define ASMULONG2(var0,var1) unsigned long var0 __asm__(""#var0"") = var1
#include <windows.h>

//static unsigned long startHigh __asm__("startHigh") = 0;
//static unsigned long startLow __asm__("startLow") = 0;
//static unsigned long endHigh __asm__("endHigh") = 0;
//static unsigned long endLow __asm__("endLow") = 0;

class ASMTimer {

private:
	bool initialized; // if the hardware supports high-resolution performance counter
	bool isTiming;    // if the timer is running currently
	bool gotTime;     // if the timer has stopped and successfully got the time

	ULARGE_INTEGER frequency; // the current performance-counter frequency, in counts per second
	ULARGE_INTEGER counter_start; // value of the performance counter at the moment when start() is called
	ULARGE_INTEGER counter_end; // value of the performance counter at the moment when end() is called
	ULARGE_INTEGER counter_ec;  // value of the error correction number.
public:

	ASMTimer();  // default constructor, perform initializations
	bool available();  // test if the very PreciseTimer instance can be used
	void start();  // start timing
	void end();    // stop timing

	UINT64 getCountsNoEC();	//get the time interval in counts.
	UINT64 getCounts();	//get the time interval in counts.
	UINT64 getMilliseconds();  // get the time interval length in milliseconds
};

inline ASMTimer::ASMTimer() {
	unsigned long startHigh = 0;
	unsigned long startLow = 0;
	unsigned long endHigh = 0;
	unsigned long endLow = 0;

	__asm__ __volatile__(
			".intel_syntax noprefix\n\t"
			"RDTSC\n\t"
			"mov        %0,     %%edx\n\t"
			"mov        %1,     %%eax\n\t"
			".att_syntax\n"
			:"=r"(startHigh),"=r"(startLow)
			:
			: "%eax","%edx"
	);
	Sleep(1000);
	__asm__ __volatile__(
			".intel_syntax noprefix\n\t"
			"RDTSC\n\t"
			"mov        %0,     %%edx\n\t"
			"mov        %1,     %%eax\n\t"
			".att_syntax\n"
			:"=r"(endHigh),"=r"(endLow)
			:
			: "%eax","%edx"
	);
	counter_start.HighPart = startHigh;
	counter_start.LowPart = startLow;
	counter_end.HighPart = endHigh;
	counter_end.LowPart = endLow;
	frequency.QuadPart = counter_end.QuadPart - counter_start.QuadPart;

	initialized = frequency.QuadPart > 0 ? true : false;

	if(!initialized){
		cout<< "Error:ASMTimer Failed!"<< endl;
	}
	isTiming = false;
	gotTime = false;
	this->start();
	this->end();
	counter_ec.QuadPart = getCountsNoEC();
}

inline void ASMTimer::start() {
	if (initialized && !isTiming) {
		isTiming = true;
		gotTime = false;
//        ASMULONG(startHigh);
//        ASMULONG(startLow);
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
	if (isTiming) {
//    	ASMULONG(endHigh);
//    	ASMULONG(endLow);
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

inline UINT64 ASMTimer::getCounts() {
	if (gotTime) {
		return (counter_end.QuadPart - counter_start.QuadPart
				- counter_ec.QuadPart);
	} else {
		return 0;
	}
}

inline bool ASMTimer::available() {
	return initialized;
}

inline UINT64 ASMTimer::getCountsNoEC() {
	if (gotTime) {
		return (counter_end.QuadPart - counter_start.QuadPart);
	} else {
		return 0;
	}
}

inline UINT64 ASMTimer::getMilliseconds() {

	if (gotTime) {
		return ((counter_end.QuadPart - counter_start.QuadPart
				- counter_ec.QuadPart)/(frequency.QuadPart/1000));
	} else {
		return 0;
	}
}

#endif /* _ASMTIMER_H_ */
