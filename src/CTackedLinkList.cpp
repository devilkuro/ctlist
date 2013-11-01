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
#include "CArrayList.h"
#include "Generator.h"

//#define CT_TEST_1
//#define CT_TEST_2
//#define CT_TEST_3
#define CT_TEST_5
//#define CT_TEST_6
//#define CT_TEST_0
#define REQUEST_NUM 10000000
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

DWORD WINAPI RecordFor6(LPVOID tmp) {
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
			CTLink* ct = new CTLink(1, 200, 86400);
			ct->CT_INDEX_THRESHOLD = 1;
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
		// 1.1st generate the request set.
		srand(0);
		for (unsigned int i = 0; i < REQUEST_NUM; i++) {
			rq[i].ts = H.U_Randint(1, 2000 - 4);
			rq[i].td = 4;
			rq[i].bw = 1;
		}
		ofstream file1("result1.log");
		file1 << "tnum/interval\t5\t10\t15\t20" << endl;
		file1.close();
		for (unsigned int i = 2; i <= 500; i += 2) {
			ofstream file("result1.log", ios::app);
			file << i;
			cout << i << endl;
			for (unsigned int j = 5; j <= 20; j += 5) {
				CTLink* ct = new CTLink(i, 2000, 2000);
				unsigned int t = 0;
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
		ofstream file2("result2.log");
		file2 << "td/tnum\t8\t16\t32\t64" << endl;
		file2.close();
		for (unsigned int i = 2; i <= 200; i += 2) {
			srand(0);
			for (unsigned int j = 0; j < REQUEST_NUM; j++) {
				rq[j].ts = H.U_Randint(1, 2000 - i);
				rq[j].td = i;
				rq[j].bw = 1;
			}
			ofstream file("result2.log", ios::app);
			file << i;
			cout << i << endl;
			for (unsigned int j = 8; j <= 64; j *= 2) {
				CTLink* ct = new CTLink(j, 2000, 2000);
				unsigned int t = 0;
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
	// TODO:add experiment 3~5.
#ifdef CT_TEST_3

#endif

	// experiment 5
#ifdef CT_TEST_5
	{
		// initialize the log file.
		ofstream file5("result5.log");
		file5 << "";
		file5.close();
		for (unsigned int i = 0; i < REQUEST_NUM; i++) {
			rq[i].td = H.U_Randint(60, 3600);
			rq[i].ts = H.U_Randint(1, 86400 - rq[i].td);
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
			CTLink* ct = new CTLink(4000, 86400, 86400);
			ct->iMaxResource = UINT_MAX;
			HANDLE hThread2 = CreateThread(NULL, 0, RecordFor6, &n, 0,
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
			CArrayList* ca = new CArrayList(86400);
			ca->max_resource = UINT_MAX;
			HANDLE hThread2 = CreateThread(NULL, 0, RecordFor6, &n, 0,
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
					CTLink* ct = new CTLink(i, 86400, 86400);
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
					// TODO add CILink if necessary
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

