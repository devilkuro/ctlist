/*
 * ASMTimer.cc
 *
 *  Created on: Feb 8, 2015
 *      Author: Fanjing-LAB
 */
#include "ASMTimer.h"
#include "iostream"

ASMTimer* ASMTimer::ptr_singleton = NULL;
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
        return;
    }
    isTiming = false;
    gotTime = false;
    this->start();
    this->end();
    counter_ec.QuadPart = getCountsNoEC();
}

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
        std::cout << counter_start.QuadPart << std::endl;
        std::cout << counter_end.QuadPart << std::endl;
    }
}

inline bool ASMTimer::available() {
    return initialized;
}

ASMTimer::~ASMTimer() {
}

ASMTimer* ASMTimer::request() {
    if(ptr_singleton == NULL){
        ptr_singleton = new ASMTimer();
    }
    return ptr_singleton;
}

void ASMTimer::release() {
    if(ptr_singleton != NULL){
        delete ptr_singleton;
        ptr_singleton = NULL;
    }
}

