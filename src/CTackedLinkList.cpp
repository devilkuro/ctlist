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

int main() {
	// TODO :1st. test the ctlink first.

	Request x;
	Helper H;
	int t1;
	int sum = 0;
	Bplus B;
	CTLink C;
	int nowtime;
	clock_t ps, start;
	//srand((unsigned)time(NULL));

	//HANDLE hThread1 = CreateThread(NULL, 0, Fun1, &A, 0, NULL);
	//CloseHandle(hThread1);
	//HANDLE hThread3 = CreateThread(NULL, 0, Fun3, &A, 0, NULL);
	//CloseHandle(hThread3);

	ps = clock();
	start = clock();//毫秒
	while(1)
	{
		sum++;

		t1 = H.P_Rand(100);
		while(clock() - start < t1);
		start = clock();//毫秒

		nowtime = clock() - ps;
		//cout<<"当前时间："<<nowtime<<endl;
		x.bw = H.U_Randint(100,1000);
		x.ts = H.U_Randint(20,50);
		x.td = (int)H.E_Rand(0.01)%100;
		C.SetTime(nowtime);
		bool flag = C.Insert(x);
		x.ts = nowtime + x.ts;
		if(flag!= B.Insert(x)){
			cout<<"ERROR!!"<<endl;
		}

	}
	return 0;
}
