/*
 * ASMTimer.cc
 *
 *  Created on: Feb 8, 2015
 *      Author: Fanjing-LAB
 */
#include "ASMTimer.h"
#include "iostream"

ASMTimer* ASMTimer::ptr_singleton = NULL;

const bool ASMTimer::available() const {
    return initialized;
}

ASMTimer::~ASMTimer() {
}

ASMTimer* ASMTimer::request() {
    if (ptr_singleton == NULL) {
        ptr_singleton = new ASMTimer();
    }
    return ptr_singleton;
}

void ASMTimer::release() {
    if (ptr_singleton != NULL) {
        delete ptr_singleton;
        ptr_singleton = NULL;
    }
}

const UINT64 ASMTimer::getCounts() const {
    if (gotTime) {
        return (counter_end.QuadPart - counter_start.QuadPart);
    } else {
        return 0;
    }
}

const UINT64 ASMTimer::getFrequency() const {
    return frequency.QuadPart;
}

const UINT64 ASMTimer::getMilliseconds() const {
    if (gotTime) {
        return ((counter_end.QuadPart - counter_start.QuadPart)
                / (frequency.QuadPart / 1000));
    } else {
        return 0;
    }
}

const INT64 ASMTimer::getECC() const {
    if (initialized) {
        return ecc;
    } else {
        return 0;
    }
}
