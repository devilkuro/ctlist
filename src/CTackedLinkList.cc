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
using namespace Fanjing;

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
void exASMTest() {
	ASMTimer* at = ASMTimer::request();
	at->start();
	at->end();
	cout << at->getCounts() << ":" << endl;
	int a = 200;
	at->start();
	Sleep(a);
	at->end();
	cout << at->getMilliseconds() << "ms@" << at->getFrequency() / 1000000000.0
			<< "GHz" << endl;
	at->release();
}
void exPoissonTest(){
    Helper h;
    int n;
    cout << "generate time:" << endl;
    cin >> n;
    for(int i = 0; i < n; ++i){
        cout << h.P_Rand(20) << endl;
    }
}
void exDevelopTest(){
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
                t_storage += timer->getCounts();
                if(n % 2 == 0){
                    n_accetp0++;
                }else{
                    n_accetp1++;
                }
            }
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
    delete gn;
}
void exStartPhaseTest() {
	// statistics parameters
	unsigned int s_Interval = 100;
	unsigned int s_Request_Num = 10000;
	// requests parameters
	unsigned int g_BW_Down = 1;
	unsigned int g_BW_Up = 20;
	unsigned int g_TS_Down = 0;
	unsigned int g_TS_Up = 36000;
	unsigned int g_TD_Avg = 128;
	unsigned int g_TD_Limit = 512;
	unsigned int g_Interval_Avg = 10;
	// initialize the test units
	BaseAdmissionController* ct[3];
	ASMTimer* timer = ASMTimer::request();
	StatisticsRecordTools* staTool = StatisticsRecordTools::request();
	Generator* gn = new Generator();
	Request* r = new Request[s_Request_Num];
	unsigned int* interval = new unsigned int[s_Request_Num];
	// run the experiment under n_round different settings : TD max~ 512,4096,32768
	for (int n_round = 0; n_round < 3;
			++n_round, g_TD_Avg *= 8, g_TD_Limit *= 8) {
		gn->setGenerator(g_BW_Down, g_BW_Up, g_TS_Down, g_TS_Up, g_TD_Avg,
				g_TD_Limit, g_Interval_Avg);
		// set requests and the intervals
		for (unsigned int i = 0; i < s_Request_Num; i++) {
			interval[i] = gn->getNext(&r[i]);
		}
		// initialize the storages: 0~CArray,1~CIlink,2~CTlink
		unsigned int max_range = g_TS_Up + g_TD_Limit;
		unsigned int index_num = max_range / g_Interval_Avg / 4;
		ct[0] = new CArrayList(max_range, 1);
		ct[1] = new CILink(index_num, max_range);
		ct[2] = new CTLink(index_num, max_range);

		// set temporary statistics variables
		unsigned int t_SetTime[3] = { 0, 0, 0 };
		unsigned int t_Accept[3] = { 0, 0, 0 };
		unsigned int t_Storage[3] = { 0, 0, 0 };
		unsigned int t_TSetTime[3] = { 0, 0, 0 };
		unsigned int t_TAccept[3] = { 0, 0, 0 };
		unsigned int t_TStorage[3] = { 0, 0, 0 };
		unsigned int t_Total[3] = { 0, 0, 0 };
		unsigned int t_nAccept[3] = { 0, 0, 0 };

		// (s_Request_Num+s_Interval-1)/s_Interval rounds out circle
		unsigned int outCircleNum = (s_Request_Num + s_Interval - 1)
				/ s_Interval;
		unsigned int endInnerCircle = s_Interval;
		unsigned int startInnerCircle = 0;
		unsigned int curCircleNum = 0;
		unsigned int oldTime = 0;
		unsigned int curTime = 0;
		bool flag = false;
		for (unsigned int ocn = 0; ocn < outCircleNum; ocn++) {
			// inner circle for different storage types;
			for (int n_type = 0; n_type < 3; ++n_type) {
				// release cpu time
				Sleep(50);
				curTime = oldTime;
				curCircleNum = startInnerCircle;
				for (; curCircleNum < endInnerCircle; curCircleNum++) {
					if (curCircleNum >= s_Request_Num) {
						cout << "ERROR!!" << endl;
					}
					// run and statistics
					curTime += interval[curCircleNum];
					timer->start();
					ct[n_type]->setTime(curTime);
					timer->end();
					t_SetTime[n_type] += timer->getCounts();
					timer->start();
					flag = ct[n_type]->accept(r[curCircleNum]);
					timer->end();
					t_Accept[n_type] += timer->getCounts();
					if (flag) {
						timer->start();
						ct[n_type]->forceInsert(r[curCircleNum]);
						timer->end();
						t_Storage[n_type] += timer->getCounts();
						t_nAccept[n_type]++;
					}
				}
				curTime = oldTime;
				curCircleNum = startInnerCircle;
			}
			// statistics process
			for (int n_type = 0; n_type < 3; ++n_type) {
				stringstream ss;
				ss << "cost per " << s_Interval
						<< " rounds:type,round,settime,accept,storage,total";
				string name = ss.str();
				ss.str("");
				staTool->changeName(name) << n_type << ocn << t_SetTime[n_type]
						<< t_Accept[n_type] << t_Storage[n_type]
						<<t_SetTime[n_type] + t_Accept[n_type]
								+ t_Storage[n_type] << staTool->endl;
				t_TSetTime[n_type] += t_SetTime[n_type];
				t_TAccept[n_type] += t_Accept[n_type];
				t_TStorage[n_type] += t_Storage[n_type];
				t_SetTime[n_type] = 0;
				t_Accept[n_type] = 0;
				t_Storage[n_type] = 0;
				t_Total[n_type] = t_TSetTime[n_type] + t_TAccept[n_type]
						+ t_TStorage[n_type];
				ss << "cost until " << endInnerCircle
						<< " rounds:type,round,settime,accept,storage,naccept,total";
				name = ss.str();
				ss.str("");
				staTool->changeName(name) << n_type << ocn << t_TSetTime[n_type]
						<< t_TAccept[n_type] << t_TStorage[n_type]
						<<  t_nAccept[n_type]<<t_Total[n_type] << staTool->endl;
			}
			oldTime = curTime;
			startInnerCircle = endInnerCircle;
			endInnerCircle =
					endInnerCircle + s_Interval > s_Request_Num ?
							s_Request_Num : endInnerCircle + s_Interval;
		}
		for (int n_type = 0; n_type < 3; ++n_type) {
			stringstream ss;
			ss << "finally cost at " << n_round
					<< " n_round:type,n_round,g_TD_Limit,settime,accept,storage,naccept,total";
			string name = ss.str();
			ss.str("");
			staTool->changeName(name) << n_type << n_round << g_TD_Limit
					<< t_TSetTime[n_type] << t_TAccept[n_type]
					<< t_TStorage[n_type] << t_nAccept[n_type]<<t_Total[n_type] << staTool->endl;
		}
	}
	staTool->output("experiment-startphase.txt", "results");
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
    	exASMTest();
    }
    if(!flagArray[EX_POISSON_TEST]){
    	exPoissonTest();
    }
    if(flagArray[EX_DEVELOP_TEST]){
    	exDevelopTest();
    }
    if(flagArray[EX_START_PHASE_TEST]){
    	exStartPhaseTest();
    }
}

