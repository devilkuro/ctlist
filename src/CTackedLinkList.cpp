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

#define CT_TEST_TIME 4305
#define CT_DEBUG_C
#define CT_DEBUG_B
#define CT_DEBUG_B_C

int main() {
	// TODO :1st. test the ctlink first.

	Request *xlist;
	unsigned int *tlist;
	Request x;
	Helper H;
	int t1;
	int sum = 0;
	CTLink C;
	Bplus B;
	int nowtime;
	clock_t ps, start;
	unsigned int startTime = clock();
	unsigned int tempTime = 0;
	xlist = new Request[MAX_REQUEST_NUM];
	tlist = new unsigned int[MAX_REQUEST_NUM];
	cout << "start to generate the data." << endl;
	for(unsigned int i = 0;i<MAX_REQUEST_NUM;i++){
		tlist[i] = H.P_Rand(100);
		xlist[i].bw = H.U_Randint(10,390);
		xlist[i].ts = H.U_Randint(200,1500);
		xlist[i].td = ((int)H.E_Rand(0.01)%100)*20+H.U_Randint(0,20);
	}
	tempTime = clock();
	cout << "generation finished." <<endl;
	cout << "generation takes " <<(tempTime-startTime)/1000.0<<" s, "<< MAX_REQUEST_NUM <<" requests have been generated."<< endl;

	tempTime = clock();
//	HANDLE hThread2 = CreateThread(NULL, 0, Fun2, &B, 0, NULL);
//	CloseHandle(hThread2);
	unsigned int staInterval = 1000000;
	ps = 0;
	start = 0;//ms
	unsigned int acceptNum = 0;
	for(unsigned int i = 0;i<MAX_REQUEST_NUM;i++)
	{
		sum++;
		t1 = tlist[i];
		start = start+t1;//ms
		x.bw = xlist[i].bw;
		x.td = xlist[i].td;
		x.ts = xlist[i].ts;
		nowtime = start - ps;
//		cout<<"now time:"<<nowtime<<endl;
//		cout<<"R("<<x.bw<<","<<x.ts<<","<<x.td<<"):"<<t1<<endl;
//		if(nowtime == CT_TEST_TIME){
//			nowtime = CT_TEST_TIME;
//		}
#ifdef CT_DEBUG_C
		C.SetTime(nowtime);
		bool flagC = C.Insert(x);
#ifndef CT_DEBUG_B_C
		if(flagC){
			acceptNum++;
		}
#endif
#endif
#ifdef CT_DEBUG_B
		x.ts = nowtime + x.ts;
		bool flagB = B.Insert(x);
		if (sum % 100 == 0) {
			if (B.root) {
//				B.FDisplay(nowtime);
				B.Delete(nowtime);
//				B.FDisplay(nowtime);
			}
		}
#ifndef CT_DEBUG_B_C
		if(flagB){
			acceptNum++;
		}
#endif
#endif
#ifdef CT_DEBUG_B_C
		if(flagC!=flagB ){
			C.Output();
			B.Output();
			cout<<"ERROR!!"<<endl;
		}
#endif

#ifndef CT_DEBUG_B_C
		if(sum%staInterval == 0){
			cout<< sum/staInterval <<":"<< clock()-tempTime<<"\taccept rate: "<<(acceptNum*1.0)/sum <<endl;
			tempTime = clock();
		}
#endif
	}
	return 0;
}
