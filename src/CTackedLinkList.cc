/*
 * CTackedLinkList.cpp
 *
 *  Created on: 2013-9-29
 *      Author: Fanjing-LAB
 */
#include "common.h"
#include "helper.h"
#include "bplus.h"
#include "ctlink.h"
#include "CILink.h"
#include "CArrayList.h"
#include "Generator.h"
#include "PreciseTimer.h"
#include "ASMTimer.h"

//#define CT_TEST_1
//#define CT_TEST_1_Z
//#define CT_TEST_2
//#define CT_TEST_3
//#define CT_TEST_3_B
//#define CT_TEST_3_C
//#define CT_TEST_4
//#define CT_TEST_4_F
//#define CT_TEST_5
//#define CT_TEST_6
#define CT_TEST_0

template<class T> string m_toStr(T tmp) {
    stringstream ss;
    ss << tmp;
    return ss.str();
}

struct ControlStack {
    // synchronizer between main thread and record thread.
    unsigned int multiple;
    unsigned int n; // the num of requests processed
    unsigned int t; // the seconds passed
    string logName; // the name of the log file
    bool stopFlag;
};

DWORD WINAPI RecordFor5(LPVOID tmp) {
    ControlStack* p = (ControlStack*) tmp;
    unsigned int pre_num = 0;
    unsigned int interval = 1000 / p->multiple;
    while(1){
        if(p->stopFlag){
            break;
        }
        ofstream file(p->logName.c_str(), ios::app);
        p->t++;
        file << p->t << "\t" << p->n << "\t" << p->n - pre_num << endl;
        pre_num = p->n;
        file.close();
        Sleep(interval); //ms
    }
    return 0;
}

int main() {
    //Helper H;
    //PreciseTimer pt;

    // for test
    {
        ASMTimer* at = ASMTimer::request();
        cout << at->getCounts() << ":" << endl;
        at->start();
        at->end();
        cout << at->getCounts() << ":" << endl;
        int a = 100;
        at->start();
        Sleep(a);
        at->end();
        cout << at->getMilliseconds() << "ms" << endl;
        at->release();
    }
}

