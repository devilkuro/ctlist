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

//#define CT_TEST_1
//#define CT_TEST_2
#define CT_TEST_3
//#define CT_TEST_3_B
#define CT_TEST_4
//#define CT_TEST_5
//#define CT_TEST_6
//#define CT_TEST_0

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
	PreciseTimer pt;

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
			//CTLink
			CTLink* ct = new CTLink(200,2000);
			unsigned int t = 0;
			cout << "CT:" << endl;
			pt.start();
			for (unsigned int k = 0; k < REQUEST_NUM; k++) {
				ct->Insert(rq[k]);
				t += 4;
				ct->SetTime(t);
				if((k+1)%100 == 0) {
					pt.end();
					cout<< pt.getMicroseconds() << endl;
					pt.start();
				}
			}
			pt.end();
			delete ct;
		}
		{
			//CILink
			CILink* ci = new CILink(200,2000);
			unsigned int t = 0;
			cout << "CI:" << endl;
			pt.start();
			for (unsigned int k = 0; k < REQUEST_NUM; k++) {
				ci->Insert(rq[k]);
				t += 4;
				ci->SetTime(t);
				if((k+1)%100 == 0) {
					pt.end();
					cout<< pt.getMicroseconds() << endl;
					pt.start();
				}
			}
			pt.end();
			delete ci;
		}
	}
#endif

	// experiment 1
#ifdef CT_TEST_1
	{
		unsigned int REQUEST_NUM=100000;
		Request* rq = new Request[REQUEST_NUM];
		ofstream file2("result1.log");
		file2 << "T/t\t4t\tc\t4i\tc\t8t\tc\t8i\tc\t16t\tc\t16i\tc\t32t\tc\t32i\tc" << endl;
		file2.close();
		unsigned int max_reserve_time = 131072;
		srand(0);
		for (unsigned int j = 0; j < REQUEST_NUM; j++) {
			rq[j].td = 1;
			rq[j].bw = 1;
			rq[j].ts = H.U_Randint(1, max_reserve_time - rq[j].td);
		}
		// i stands for T in cost formula
		for (unsigned int i = 4; i <= 1024; i += 4) {
			ofstream file("result1.log", ios::app);
			file << i;
			cout << i << endl;
			// j stands for t in cost formula
			for (unsigned int j = 4; j <= 32; j *= 2) {
				{
					//CTLink
					CTLink* ct = new CTLink(max_reserve_time / i,
							max_reserve_time);
					unsigned int t = 0;
					// 1st. first round to fill the CTLink
					for (unsigned int k = 0; k < REQUEST_NUM; k++) {
						ct->Insert(rq[k]);
						t += j;
						ct->SetTime(t);
						// the CTLink is full.
						if (t > max_reserve_time) {
							break;
						}
					}
					// 2nd. second round to test the performance
					pt.start();
					for (unsigned int k = 0; k < REQUEST_NUM; k++) {
						ct->Insert(rq[k]);
						t += j;
						ct->SetTime(t);
					}
					pt.end();
					file << "\t" << pt.getMicroseconds() << "\t"
							<< pt.getCounts();
					delete ct;
				}
				{
					//CILink
					CILink* ci = new CILink(max_reserve_time / i,
							max_reserve_time);
					unsigned int t = 0;
					// 1st. first round to fill the CTLink
					for (unsigned int k = 0; k < REQUEST_NUM; k++) {
						ci->Insert(rq[k]);
						t += j;
						ci->SetTime(t);
						// the CTLink is full.
						if (t > max_reserve_time) {
							break;
						}
					}
					// 2nd. second round to test the performance
					pt.start();
					for (unsigned int k = 0; k < REQUEST_NUM; k++) {
						ci->Insert(rq[k]);
						t += j;
						ci->SetTime(t);
					}
					pt.end();
					file << "\t" << pt.getMicroseconds() << "\t"
							<< pt.getCounts();
					delete ci;
				}
			}
			file << endl;
			file.close();
		}
	}
#endif

	// experiment 2
#ifdef CT_TEST_2

	{
		unsigned int REQUEST_NUM=100000;
		Request* rq = new Request[REQUEST_NUM];
		ofstream file1("result2.log");
		file1 << "TD/T\t128\tc\t256\tc\t512\tc\t1024\tc" << endl;
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
				pt.start();
				for (unsigned int k = 0; k < REQUEST_NUM; k++) {
					ct->Insert(rq[k]);
					t += 4;
					ct->SetTime(t);
				}
				pt.end();
				file << "\t" << pt.getMicroseconds() <<  "\t" << pt.getCounts();
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
		unsigned int REQUEST_NUM=100000;
		Request* rq = new Request[REQUEST_NUM];
		// 1.initialize the log file
		ofstream file4("result3.log");
		file4 << "";
		file4.close();
		unsigned int max_reserve_time = 32768;
		srand(0);
		for (unsigned int i = 0; i < REQUEST_NUM; i++) {
			rq[i].ts = H.U_Randint(1, max_reserve_time - 4);
			rq[i].td = 4;
			rq[i].bw = 1;
		}
		ofstream file("result3.log", ios::app);
		for (int i = 4; i <= 4096; i+=4) {
			{
				//CTLink
				CTLink* ct = new CTLink(max_reserve_time/i,max_reserve_time);
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
				pt.start();
				for (unsigned int k = 0; k < REQUEST_NUM; k++) {
					ct->Insert(rq[k]);
					t += 4;
					ct->SetTime(t);
				}
				pt.end();
				file << i << "\tCT" <<"\t" << pt.getMicroseconds() <<  "\t" << pt.getCounts() << endl;
				delete ct;
			}
		}

		for (int i = 4; i <= 4096; i+=4) {
			{
				//CILink
				CILink* ci = new CILink(max_reserve_time/i,max_reserve_time);
				unsigned int t = 0;
				// 1st. first round to fill the CTLink
				for (unsigned int k = 0; k < REQUEST_NUM; k++) {
					ci->Insert(rq[k]);
					t += 4;
					ci->SetTime(t);
					// the CTLink is full.
					if(t>max_reserve_time){
						break;
					}
				}
				pt.start();
				for (unsigned int k = 0; k < REQUEST_NUM; k++) {
					ci->Insert(rq[k]);
					t += 4;
					ci->SetTime(t);
				}
				pt.end();
				file << i << "\tCI" << "\t" << pt.getMicroseconds() <<  "\t" << pt.getCounts() << endl;
				delete ci;
			}
		}
		file.close();
	}
#endif
	// experiment 3 b
#ifdef CT_TEST_3_B
	{
		unsigned int REQUEST_NUM=5000;
		Request* rq = new Request[REQUEST_NUM];
		// 1.initialize the log file
		ofstream file3("result3.log");
		file3 << "ID\tus\tc"<< endl;
		file3.close();
		unsigned int max_reserve_time = 65536;
		srand(0);
		for (unsigned int i = 0; i < REQUEST_NUM; i++) {
			rq[i].td = 2;
			rq[i].bw = 1;
			rq[i].ts = H.U_Randint(1, max_reserve_time - rq[i].td);
		}
		ofstream file("result3.log", ios::app);
		for (int i = 512; i <= 8192; i*=2) {
			cout<< i<<endl;
			{
				//CILink
				CTLink* ct[1000];
				for (int var = 0; var < 1000; ++var) {
					ct[var] = new CTLink(i, max_reserve_time);
				}
				unsigned int t = 0;
				unsigned int k = 0;
				unsigned int x = 20;
				pt.start();
				for (; k < REQUEST_NUM; k++) {
					x--;
					for (int var = 0; var < 1000; ++var) {
						ct[var]->Insert(rq[k]);
						t += 4;
						ct[var]->SetTime(t);
					}
					if(x==0){
						pt.end();
						x=20;
						file << i << "\tCT" << "\t" << pt.getMicroseconds()
								<< "\t" << pt.getCounts() << endl;
						pt.start();
					}
				}
				pt.end();
				for (int var = 0; var < 1000; ++var) {
					delete ct[var];
				}
			}

			{
				//CILink
				CILink* ci[1000];
				for (int var = 0; var < 1000; ++var) {
					ci[var] = new CILink(i, max_reserve_time);
				}
				unsigned int t = 0;
				unsigned int k = 0;
				unsigned int x = 20;
				pt.start();
				for (; k < REQUEST_NUM; k++) {
					x--;
					for (int var = 0; var < 1000; ++var) {
						ci[var]->Insert(rq[k]);
						t += 4;
						ci[var]->SetTime(t);
					}
					if (x == 0) {
						pt.end();
						x = 20;
						file << i << "\tCI" << "\t" << pt.getMicroseconds()
								<< "\t" << pt.getCounts() << endl;
						pt.start();
					}
				}
				pt.end();
				for (int var = 0; var < 1000; ++var) {
					delete ci[var];
				}
			}
		}
		file.close();
	}
#endif

	// experiment 4
#ifdef CT_TEST_4
	{
		unsigned int REQUEST_NUM=100000;
		Request* rq = new Request[REQUEST_NUM];
		// 1.initialize the log file
		ofstream file4("result4.log");
		file4 << "";
		file4.close();
		unsigned int max_reserve_time = 32768;
		srand(0);
		for (unsigned int i = 0; i < REQUEST_NUM; i++) {
			rq[i].ts = H.U_Randint(1, max_reserve_time - 4);
			rq[i].td = 4;
			rq[i].bw = 1;
		}
		ofstream file("result4.log", ios::app);
		for (int i = 256; i <= 4096; i*=2) {
			{
				//CTLink
				CTLink* ct = new CTLink(i,max_reserve_time);
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
				pt.start();
				for (unsigned int k = 0; k < REQUEST_NUM; k++) {
					ct->Insert(rq[k]);
					t += 4;
					ct->SetTime(t);
				}
				pt.end();
				file << i << "\tCT" <<"\t" << pt.getMicroseconds() <<  "\t" << pt.getCounts() << endl;
				delete ct;
			}
			{
				//CILink
				CILink* ci = new CILink(i,max_reserve_time);
				unsigned int t = 0;
				// 1st. first round to fill the CTLink
				for (unsigned int k = 0; k < REQUEST_NUM; k++) {
					ci->Insert(rq[k]);
					t += 4;
					ci->SetTime(t);
					// the CTLink is full.
					if(t>max_reserve_time){
						break;
					}
				}
				pt.start();
				for (unsigned int k = 0; k < REQUEST_NUM; k++) {
					ci->Insert(rq[k]);
					t += 4;
					ci->SetTime(t);
				}
				pt.end();
				file << i << "\tCI" << "\t" << pt.getMicroseconds() <<  "\t" << pt.getCounts() << endl;
				delete ci;
			}
		}
		file.close();
	}
#endif

	// experiment 5
#ifdef CT_TEST_5
	{
		unsigned int REQUEST_NUM=100000;
		Request* rq = new Request[REQUEST_NUM];
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
		unsigned int REQUEST_NUM=100000;
		Request* rq = new Request[REQUEST_NUM];
		unsigned int max_reserve_time = 86400;
		srand(0);
		unsigned int *interval = new unsigned int[REQUEST_NUM];
		for (unsigned int i = 0; i < REQUEST_NUM; i++) {
			rq[i].td = H.U_Randint(60, 3600);
			rq[i].ts = H.U_Randint(1, max_reserve_time - rq[i].td);
			rq[i].bw = H.U_Randint(6, 16);
			interval[i] = H.U_Randint(5, 15);
		}
		ofstream file6("result6.log");
		file6 << "tnum/cost\tCTLink\tCIlink" << endl;
		file6.close();
		{
			for (unsigned int i = 256; i <= 8192; i *= 2) {
				ofstream file("result6.log", ios::app);
				file << i;
				cout << i << endl;
				{
					//CTLink
					CTLink* ct = new CTLink(i, max_reserve_time);
					unsigned int t = 0;
					pt.start();
					for (unsigned int k = 0; k < REQUEST_NUM; k++) {
						ct->Insert(rq[k]);
						t += interval[k];
						ct->SetTime(t);
					}
					pt.end();
					file << "\t" << pt.getMilliseconds() <<"\t"<<pt.getCounts()<<"\t"<<pt.getCountsEC();
					delete ct;
				}
				{
					// CILink
					CILink* ci = new CILink(i, max_reserve_time);
					unsigned int t = 0;
					pt.start();
					for (unsigned int k = 0; k < REQUEST_NUM; k++) {
						ci->Insert(rq[k]);
						t += interval[k];
						ci->SetTime(t);
					}
					pt.end();
					file << "\t" << pt.getMilliseconds() <<"\t"<<pt.getCounts()<<"\t"<<pt.getCountsEC();
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
			CArrayList* ca = new CArrayList(max_reserve_time);
			unsigned int t = 0;
			pt.start();
			for (unsigned int k = 0; k < REQUEST_NUM; k++) {
				ca->Insert(rq[k]);
				t += interval[k];
				ca->setTime(t);
			}
			pt.end();
			file << "\t" << pt.getMilliseconds() <<"\t"<<pt.getCounts()<<"\t"<<pt.getCountsEC();
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
			pt.start();
			for (unsigned int k = 0; k < REQUEST_NUM; k++) {
				rq[k].ts += t;
				bp->Insert(rq[k]);
				t += interval[k];
				if ((k + 1) % 100 == 0) {
					bp->Delete(t);
				}
			}
			pt.end();
			file << "\t" << pt.getMilliseconds() <<"\t"<<pt.getCounts()<<"\t"<<pt.getCountsEC();
			delete bp;
			file << endl;
			file.close();
		}
	}
#endif
}

