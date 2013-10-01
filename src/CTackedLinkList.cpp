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

Bplus B;
int nowtime;
//DWORD WINAPI Fun2(LPVOID p)
//{
//	while(1)
//	{
//		if(B.root)
//		{
//			B.FDisplay(nowtime);
//			B.Delete(nowtime);
//			B.FDisplay(nowtime);
//		}
//		Sleep(1000);//ms
//	}
//	return 0;
//}
int main() {
	// TODO :1st. test the ctlink first.

	Request x;
	Helper H;
	int t1;
	int sum = 0;
	CTLink C;
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
		cout<<t1<<endl;
		start = start+t1;//ms

		nowtime = start - ps;
		if(nowtime == 4305){
			nowtime = 4305;
		}
		cout<<"now time:"<<nowtime<<endl;
		x.bw = H.U_Randint(100,1000);
		x.ts = H.U_Randint(20,50);
		x.td = (int)H.E_Rand(0.01)%100;
		C.SetTime(nowtime);
		/*bool flag = */C.Insert(x);
		x.ts = nowtime + x.ts;
//		if(flag!= B.Insert(x)){
//			cout<<"ERROR!!"<<endl;
//		}

	}
	return 0;
}
