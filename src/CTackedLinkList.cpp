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
//#define CT_DEBUG_C
#define CT_DEBUG_B

int main() {
	// TODO :1st. test the ctlink first.

	Request x;
	Helper H;
	int t1;
	int sum = 0;
	CTLink C;
	Bplus B;
	int nowtime;
	clock_t ps, start;

//	HANDLE hThread2 = CreateThread(NULL, 0, Fun2, &B, 0, NULL);
//	CloseHandle(hThread2);

	ps = 0;
	start = 0;//ms
	while(1)
	{
		sum++;

		t1 = H.P_Rand(100);
		start = start+t1;//ms

		x.bw = H.U_Randint(100,1000);
		x.ts = H.U_Randint(20,50);
		x.td = (int)H.E_Rand(0.01)%100;
		nowtime = start - ps;
		cout<< sum << endl;
//		cout<<"now time:"<<nowtime<<endl;
//		cout<<"R("<<x.bw<<","<<x.ts<<","<<x.td<<"):"<<t1<<endl;
//		if(nowtime == CT_TEST_TIME){
//			nowtime = CT_TEST_TIME;
//		}
#ifdef CT_DEBUG_C
		C.SetTime(nowtime);
		bool flagC = C.Insert(x);
#endif
#ifdef CT_DEBUG_B
		x.ts = nowtime + x.ts;
		bool flagB = B.Insert(x);
		if (sum % 10 == 0) {
			if (B.root) {
//				cout<<"before:"<<endl;
//				cout<<"output:"<<endl;
//				B.Output();
//				cout<<"traverse:"<<endl;
//				B.Traverse(B.root);
				B.Delete(nowtime);
//				cout<<"after:"<<endl;
//				cout<<"output:"<<endl;
//				B.Output();
//				cout<<"traverse:"<<endl;
//				B.Traverse(B.root);
			}
			Sleep(1);
		}
		int interval = 5000;
		if (sum % interval == interval-1){
			B.Delete(nowtime+300);
			B.Output();
			cin >> sum;
		}
#endif
#ifdef CT_DEBUG_C
#ifdef CT_DEBUG_B
		if(flagC!=flagB ){
			C.Output();
			B.Output();
			cout<<"ERROR!!"<<endl;
		}
#endif
#endif
	}
	return 0;
}
