/*
 * array.h
 *
 *  Created on: 2013-9-18
 *      Author: Fanjing
 */

#ifndef _ARRAY_H_
#define _ARRAY_H_

#include "common.h"

class Array {
public:
    Array(int n);
    bool Insert(Request r);
    void Clear();
    void Display();
    static int sumA;
    static unsigned int t;
private:
    int* p;
    unsigned int N;
};

#endif /* ARRAY_H_ */
