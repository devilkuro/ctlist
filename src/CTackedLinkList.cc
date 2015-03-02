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
    enum ExperimentCode {
        EX_ASM_TEST = 0,
        EX_POISSON_TEST,
        EX_ARRAY_TEST,
        EX_LAST_FLAG
    };
    string flagStrArray[] = {
            "EX_ASM_TEST",
            "EX_POISSON_TEST",
            "EX_ARRAY_TEST",
            "EX_LAST_FLAG" };
    bool *flagArray = new bool[EX_LAST_FLAG];
    {
        int flag = 0;
        cout << "Input flag for each experiment: 1 for run, 0 for not." << endl;
        for(int i = 0; i < EX_LAST_FLAG; ++i){
            cout << flagStrArray[i] << endl;
            cin >> flag;
            if(flag == 1){
                flagArray[i] = true;
            }else if(flag == 0){
                flagArray[i] = false;
            }
        }
    }

    // for test
    if(flagArray[EX_LAST_FLAG]){
        ASMTimer* at = ASMTimer::request();
        at->start();
        at->end();
        cout << at->getCounts() << ":" << endl;
        int a = 200;
        at->start();
        Sleep(a);
        at->end();
        cout << at->getMilliseconds() << "ms@"
                << at->getFrequency() / 1000000000.0 << "GHz" << endl;
        at->release();
    }
    if(flagArray[EX_POISSON_TEST]){
        Helper h;
        int n;
        cout<<"generate time:"<<endl;
        cin>>n;
        for (int i = 0; i < n; ++i) {
            cout<< h.P_Rand(20)<<endl;
        }
    }
    if(flagArray[EX_ARRAY_TEST]){

    }
}

