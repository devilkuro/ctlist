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
#include "StatisticsRecordTools.h"
//#include "stdlib.h"

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
        EX_DEVELOP_TEST,
        EX_START_PHASE_TEST,
        EX_LAST_FLAG
    };
    string flagStrArray[] = {
            "EX_ASM_TEST",
            "EX_POISSON_TEST",
            "EX_DEVELOP_TEST",
            "EX_START_PHASE_TEST",
            "EX_LAST_FLAG" };
    bool *flagArray = new bool[EX_LAST_FLAG];
    {
        int flag = 0;
        cout << "Input flag for each experiment: 1 for run, 0 for not." << endl;
        for(int i = 0; i < EX_LAST_FLAG; ++i){
            cout << flagStrArray[i] << endl;
            //cin >> flag;
            flag = 1;
            if(flag == 1){
                flagArray[i] = true;
            }else if(flag == 0){
                flagArray[i] = false;
            }
        }
    }

    // for test
    if(!flagArray[EX_ASM_TEST]){
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
    if(!flagArray[EX_POISSON_TEST]){
        Helper h;
        int n;
        cout << "generate time:" << endl;
        cin >> n;
        for(int i = 0; i < n; ++i){
            cout << h.P_Rand(20) << endl;
        }
    }
    if(flagArray[EX_DEVELOP_TEST]){
        Generator* gn = new Generator();
        gn->setGenerator(1, 20, 0, 36000, 800, 36000, 10);
        BaseAdmissionController* ct;
        ASMTimer* timer = ASMTimer::request();
        unsigned int t_total0 = 0;
        unsigned int t_total1 = 0;
        unsigned int t_setTime = 0;
        unsigned int t_accept = 0;
        unsigned int t_storage = 0;
        unsigned int n_accetp0 = 0;
        unsigned int n_accetp1 = 0;
        unsigned int curTime = 0;
        Request* r = new Request[100000];
        unsigned int* interval = new unsigned int[100000];

        for(int i = 0; i < 100000; i++){
            interval[i] = gn->getNext(&r[i]);
        }
        cout<<r[1000].bw<<endl;
        for(int n = 0; n < 2; ++n){
            Sleep(50);
            //t_total = 0;
            t_setTime = 0;
            t_accept = 0;
            t_storage = 0;
            curTime = 0;
            CTLink* ct0 = new CTLink(720, 72000);
            ct0->iMaxResource = 200;
            CILink* ci0 = new CILink(720, 72000);
            ci0->iMaxResource = 200;
            if(n % 2 == 0){
                ct = dynamic_cast<BaseAdmissionController*>(ct0);
            }else{
                ct = dynamic_cast<BaseAdmissionController*>(ci0);
            }
            //Request temp;
            bool flag = true;
            for(int i = 0; i < 1000; i++){
                curTime += interval[i];
                timer->start();
                ct->setTime(curTime);
                timer->end();
                t_setTime += timer->getCounts();
                timer->start();
                flag = ct->accept(r[i]);
                timer->end();
                t_accept += timer->getCounts();
                if(flag){
                    timer->start();
                    ct->forceInsert(r[i]);
                    timer->end();
                    if(n % 2 == 0){
                        n_accetp0++;
                    }else{
                        n_accetp1++;
                    }
                }
                t_storage += timer->getCounts();
            }
            if(n % 2 == 0){
                t_total0 += (t_setTime + t_storage + t_accept) / 10;
            }else{
                t_total1 += (t_setTime + t_storage + t_accept) / 10;
            }
            cout << (n % 2 == 0 ? "ct:" : "ci:") << endl;
            cout << "settime: " << t_setTime << endl;
            cout << "accept: " << t_accept << endl;
            cout << "storage: " << t_storage << endl;
            delete ct0;
            delete ci0;
        }
        cout << "ct: " << t_total0 << "\t" << n_accetp0 << endl;
        cout << "ci: " << t_total1 << "\t" << n_accetp1 << endl;
        delete interval;
        delete[] r;
    }
    if(!flagArray[EX_START_PHASE_TEST]){
        // statistics parameters
        unsigned int s_Interval;
        // requests parameters
        unsigned int g_BW_Down = 1;
        unsigned int g_BW_Up = 20;
        unsigned int g_TS_Down = 0;
        unsigned int g_TS_Up = 36000;
        unsigned int g_TD_Avg = 80;
        unsigned int g_TD_Limit = 4000;
        unsigned int g_Interval_Avg = 10;
    }
}

