/*
 * ASMTimer.cc
 *
 *  Created on: Feb 8, 2015
 *      Author: Fanjing-LAB
 */
#include "ASMTimer.h"
#include "iostream"

ASMTimer* ASMTimer::ptr_singleton = NULL;
ASMTimer::ASMTimer() {
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
}

const bool ASMTimer::available() const {
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

const UINT64 ASMTimer::getCounts() const {
    if(gotTime){
        return (counter_end.QuadPart - counter_start.QuadPart);
    }else{
        return 0;
    }
}

const UINT64 ASMTimer::getFrequency() const {
    return frequency.QuadPart;
}

const UINT64 ASMTimer::getMilliseconds() const {
    if(gotTime){
        return ((counter_end.QuadPart - counter_start.QuadPart)
                / (frequency.QuadPart / 1000));
    }else{
        return 0;
    }
}
