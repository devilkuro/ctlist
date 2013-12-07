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

//#define CT_TEST_1
//#define CT_TEST_2
//#define CT_TEST_3
//#define CT_TEST_5
#define CT_TEST_6
//#define CT_TEST_0
#define REQUEST_NUM 1000000
template<class T> string m_toStr(T tmp) {
	stringstream ss;
	ss << tmp;
	return ss.str();
}

struct ControlStack {
	// synchronizer between main thread and record thread.
	unsigned int n; // the num of requests processed
	unsigned int t; // the seconds passed
	string logName; // the name of the log file
	bool stopFlag;
};

DWORD WINAPI RecordFor5(LPVOID tmp) {
	ControlStack* p = (ControlStack*) tmp;
	unsigned int pre_num=0;
	while (1) {
		if (p->stopFlag) {
			break;
		}
		ofstream file(p->logName.c_str(), ios::app);
		p->t++;
		file << p->t << "\t" << p->n << "\t" << p->n - pre_num << endl;
		pre_num = p->n;
		file.close();
		Sleep(100); //ms
	}
	return 0;
}

int main() {
	Helper H;
	Request* rq = new Request[REQUEST_NUM];

	// for test
#ifdef CT_TEST_0
	{
		// 1.1st generate the request set.
		srand(0);
		for (unsigned int i = 0; i < REQUEST_NUM; i++) {
			rq[i].ts = H.U_Randint(1, 2000 - 4);
			rq[i].td = 4;
			rq[i].bw = 1;
		}
		{
			//CILink
			CTLink* ct = new CTLink(1,86400);
			unsigned int t = 0;
			clock_t start = clock();
			for (unsigned int k = 0; k < REQUEST_NUM; k++) {
				ct->Insert(rq[k]);
				t += 4;
				ct->SetTime(t);
				if((k+1)%10000 == 0) {
					cout<< k << endl;
				}
			}
			cout << "\t" << clock() - start;
			delete ct;
		}
	}
#endif

	// experiment 1
#ifdef CT_TEST_1

	{
		ofstream file2("result1.log");
		file2 << "TD/ITV\t4\t8\t16\t32" << endl;
		file2.close();
		unsigned int max_reserve_time = 131072;
		srand(0);
		for (unsigned int j = 0; j < REQUEST_NUM; j++) {
			rq[j].ts = H.U_Randint(1, max_reserve_time - 4);
			rq[j].td = 1;
			rq[j].bw = 1;
		}
		// i stands for T in cost formula
		for (unsigned int i = 4; i <= 1024; i += 4) {
			ofstream file("result1.log", ios::app);
			file << i;
			cout << i << endl;
			// j stands for t in cost formula
			for (unsigned int j = 4; j <= 32; j *= 2) {
				CTLink* ct = new CTLink(max_reserve_time/i,max_reserve_time);
				unsigned int t = 0;
				// 1st. first round to fill the CTLink
				for (unsigned int k = 0; k < REQUEST_NUM; k++) {
					ct->Insert(rq[k]);
					t += j;
					ct->SetTime(t);
					// the CTLink is full.
					if(t>max_reserve_time){
						break;
					}
				}
				// 2nd. second round to test the performance
				clock_t start = clock();
				for (unsigned int k = 0; k < REQUEST_NUM; k++) {
					ct->Insert(rq[k]);
					t += j;
					ct->SetTime(t);
				}
				file << "\t" << clock() - start;
				delete ct;
			}
			file << endl;
			file.close();
		}
	}
#endif

	// experiment 2
#ifdef CT_TEST_2

	{
		ofstream file1("result2.log");
		file1 << "TD/T\t128\t256\t512\t1024" << endl;
		file1.close();
		unsigned int max_reserve_time = 131072;
		for (unsigned int i = 256; i <= 2048; i += 8) {
			ofstream file("result2.log", ios::app);
			file << i;
			cout << i << endl;
			// generate the request set.
			srand(0);
			for (unsigned int k = 0; k < REQUEST_NUM; k++) {
				rq[k].ts = H.U_Randint(1, max_reserve_time - i);
				rq[k].td = i;
				rq[k].bw = 1;
			}
			for (unsigned int j = 128; j <= 1024; j *=2) {
				CTLink* ct = new CTLink(max_reserve_time/j, max_reserve_time);
				unsigned int t = 0;
				// 1st. first round to fill the CTLink
				for (unsigned int k = 0; k < REQUEST_NUM; k++) {
					ct->Insert(rq[k]);
					t += 4;
					ct->SetTime(t);
					// the CTLink is full.
					if(t>max_reserve_time){
						break;
					}
				}
				// 2nd. second round to test the performance
				clock_t start = clock();
				for (unsigned int k = 0; k < REQUEST_NUM; k++) {
					ct->Insert(rq[k]);
					t += 4;
					ct->SetTime(t);
				}
				file << "\t" << clock() - start;
				delete ct;
			}
			file << endl;
			file.close();
		}
	}

#endif

	// experiment 3
#ifdef CT_TEST_3
	{
		// 1.initialize the log file
		ofstream file3("result3.log");
		file3 << "";
		file3.close();
		srand(0);
		unsigned int max_reserve_time = 1048576;
		for(unsigned int interval = 16;interval<=128;interval+=16){
			for (unsigned int duration = 4; duration <= 64; duration += 4) {
				// 2.1st generate the request set.
				for (unsigned int i = 0; i < REQUEST_NUM; i++) {
					rq[i].ts = H.U_Randint(1, max_reserve_time - duration);
					rq[i].td = duration;
					rq[i].bw = 1;
				}
				// 2.2nd calculate the result and record into the log file.
				CTLink* ct = new CTLink(max_reserve_time/interval, max_reserve_time);
				unsigned int t = 0;
				ofstream file("result3.log", ios::app);
				clock_t start = clock();
				for (int j = 0; j < 10; ++j) {
					for (unsigned int k = 0; k < REQUEST_NUM; k++) {
						ct->Insert(rq[k]);
						t += 4;
						ct->SetTime(t);
					}
				}
				file << "T: "<<interval<<"; t: 4; d: "<< duration<<"; -> coat: " << clock() - start<<endl;
				file.close();
				delete ct;
			}
		}
	}
#endif
	// TODO:add experiment 4~5.

	// experiment 5
#ifdef CT_TEST_5
	{
		// initialize the log file.
		unsigned int max_reserve_time = 864000;
		ofstream file5("result5.log");
		file5 << "";
		file5.close();
		for (unsigned int i = 0; i < REQUEST_NUM; i++) {
			rq[i].td = H.U_Randint(60, 3600);
			rq[i].ts = H.U_Randint(1, max_reserve_time - rq[i].td);
			rq[i].bw = 1;
		}
		{
			//CTLink
			ofstream file("result5.log", ios::app);
			file << "CTLink" << endl;
			file << "TIME\t" << "NUM\t" << "DIFF" << endl;
			file.close();

			ControlStack n;
			n.logName = "result5.log";
			n.t = 0;
			n.n = 0;
			n.stopFlag = false;
			CTLink* ct = new CTLink(160000,max_reserve_time);
			ct->iMaxResource = UINT_MAX;
			HANDLE hThread2 = CreateThread(NULL, 0, RecordFor5, &n, 0,
			NULL);
			for (unsigned int k = 0; k < REQUEST_NUM; k++) {
				ct->Insert(rq[k]);
				ct->SetTime(n.t/10);
				n.n++;
				if (n.t < 200){
					if (k == REQUEST_NUM - 1) {
						k = 0;
					}
				} else {
					n.stopFlag = true;
					break;
				}
			}
			CloseHandle(hThread2);
			delete ct;
		}
		// sleep 200ms to wait the RecordThread to stop
		Sleep(200);
		{
			//CArrayList
			ofstream file("result5.log", ios::app);
			file << "CArrayList" << endl;
			file << "TIME\t" << "NUM\t" << "DIFF" << endl;
			file.close();

			ControlStack n;
			n.logName = "result5.log";
			n.t = 0;
			n.n = 0;
			n.stopFlag = false;
			CArrayList* ca = new CArrayList(max_reserve_time);
			ca->max_resource = UINT_MAX;
			HANDLE hThread2 = CreateThread(NULL, 0, RecordFor5, &n, 0,
			NULL);
			for (unsigned int k = 0; k < REQUEST_NUM; k++) {
				ca->Insert(rq[k]);
				ca->setTime(n.t/10);
				n.n++;
				if (n.t < 200){
					if (k == REQUEST_NUM - 1) {
						k = 0;
					}
				} else {
					n.stopFlag = true;
					break;
				}
			}
			CloseHandle(hThread2);
			delete ca;
		}
		// sleep 200ms to wait the RecordThread to stop
		Sleep(200);
		{
			//Bplus
			ofstream file("result5.log", ios::app);
			file << "Bplus" << endl;
			file << "TIME\t" << "NUM\t" << "DIFF" << endl;
			file.close();

			ControlStack n;
			n.logName = "result5.log";
			n.t = 0;
			n.n = 0;
			n.stopFlag = false;
			Bplus* bp = new Bplus();
			Request tmpR;

			bp->RMAX = UINT_MAX;
			HANDLE hThread2 = CreateThread(NULL, 0, RecordFor5, &n, 0,
			NULL);
			for (unsigned int k = 0; k < REQUEST_NUM; k++) {
				tmpR.bw = rq[k].bw;
				tmpR.td = rq[k].td;
				tmpR.ts = rq[k].ts+n.t/10;
				bp->Insert(tmpR);
				bp->Delete(n.t/10);
				n.n++;
				if (n.t < 200){
					if (k == REQUEST_NUM - 1) {
						k = 0;
					}
				} else {
					n.stopFlag = true;
					break;
				}
			}
			CloseHandle(hThread2);
			delete bp;
		}
	}
#endif

	// experiment 6
#ifdef CT_TEST_6

	{
		srand(0);
		unsigned int *interval = new unsigned int[REQUEST_NUM];
		for (unsigned int i = 0; i < REQUEST_NUM; i++) {
			rq[i].td = H.U_Randint(60, 3600);
			rq[i].ts = H.U_Randint(1, 86400 - rq[i].td);
			rq[i].bw = H.U_Randint(6, 16);
			interval[i] = H.U_Randint(10, 30);
		}
		ofstream file6("result6.log");
		file6 << "tnum/cost\tCTLink\tCIlink" << endl;
		file6.close();
		{
			for (unsigned int i = 4; i <= 2048; i *= 2) {
				ofstream file("result6.log", ios::app);
				file << i;
				cout << i << endl;
				{
					//CTLink
					CTLink* ct = new CTLink(i, 86400);
					unsigned int t = 0;
					clock_t start = clock();
					for (unsigned int k = 0; k < REQUEST_NUM; k++) {
						ct->Insert(rq[k]);
						t += interval[k];
						ct->SetTime(t);
					}
					file << "\t" << clock() - start;
					delete ct;
				}
				{
					// CILink
					CILink* ci = new CILink(i, 86400);
					unsigned int t = 0;
					clock_t start = clock();
					for (unsigned int k = 0; k < REQUEST_NUM; k++) {
						ci->Insert(rq[k]);
						t += interval[k];
						ci->SetTime(t);
					}
					file << "\t" << clock() - start;

					delete ci;
				}
				file << endl;
				file.close();
			}
		}
		//CArrayList
		{
			ofstream file("result6.log", ios::app);
			cout << "CArrayList:" << endl;
			CArrayList* ca = new CArrayList(86400);
			unsigned int t = 0;
			clock_t start = clock();
			for (unsigned int k = 0; k < REQUEST_NUM; k++) {
				ca->Insert(rq[k]);
				t += interval[k];
				ca->setTime(t);
			}
			file << "\t" << clock() - start;
			delete ca;
			file << endl;
			file.close();
		}
		//Bplus
		{
			ofstream file("result6.log", ios::app);
			cout << "Bplus:" << endl;
			Bplus* bp = new Bplus();
			unsigned int t = 0;
			clock_t start = clock();
			for (unsigned int k = 0; k < REQUEST_NUM; k++) {
				rq[k].ts += t;
				bp->Insert(rq[k]);
				t += interval[k];
				if ((k + 1) % 1000 == 0) {
					bp->Delete(t);
				}
			}
			file << "\t" << clock() - start;
			delete bp;
			file << endl;
			file.close();
		}
	}
#endif
}

