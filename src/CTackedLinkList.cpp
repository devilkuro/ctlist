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

#define REQUEST_NUM 10000000
template <class T> string m_toStr(T tmp)
{
    stringstream ss;
    ss << tmp;
    return ss.str();
}
int main(){
	Helper H;
	Request* rq = new Request[REQUEST_NUM];
/*
	// experiment 1
	// 1.1st generate the request set.

	srand(0);
	for(unsigned int i = 0;i<REQUEST_NUM;i++){
		rq[i].ts = H.U_Randint(1,2000-4);
		rq[i].td = 4;
		rq[i].bw = 1;
	}
	ofstream file1("result1.txt");
	file1 << "tnum/interval\t5\t10\t15\t20" <<endl;
	file1.close();
	for(unsigned int i = 2;i<=500;i+=2){
		ofstream file("result1.txt",ios::app);
		file << i ;
		cout << i << endl;
		for(unsigned int j = 5;j<=20;j+=5){
			CTLink* ct = new CTLink(i,2000,2000);
			unsigned int t = 0;
			clock_t start = clock();
			for(unsigned int k = 0;k<REQUEST_NUM;k++){
				ct->Insert(rq[k]);
				t+=j;
				ct->SetTime(t);
			}
			file << "\t" << clock() - start;
			delete ct;
		}
		file << endl;
		file.close();
	}
*/

	ofstream file2("result2.txt");
	file2 << "td/tnum\t8\t16\t32\t64" <<endl;
	file2.close();
	// experiment 2
	for(unsigned int i = 2;i<=200;i+=2){
		srand(0);
		for(unsigned int j = 0;j<REQUEST_NUM;j++){
			rq[j].ts = H.U_Randint(1,2000-i);
			rq[j].td = i;
			rq[j].bw = 1;
		}
		ofstream file("result2.txt",ios::app);
		file << i ;
		cout << i << endl;
		for(unsigned int j = 8;j<=64;j*=2){
			CTLink* ct = new CTLink(j,2000,2000);
			unsigned int t = 0;
			clock_t start = clock();
			for(unsigned int k = 0;k<REQUEST_NUM;k++){
				ct->Insert(rq[k]);
				t+=1;
				ct->SetTime(t);
			}
			file << "\t" << clock() - start;
			delete ct;
		}
		file << endl;
		file.close();
	}
	// experiment 3
	// TODO:add experiment 3~5.

	// experiment 6
	srand(0);
	unsigned int *interval = new unsigned int[REQUEST_NUM];
	for (unsigned int i = 0; i < REQUEST_NUM; i++) {
		rq[i].td = H.U_Randint(60,3600);
		rq[i].ts = H.U_Randint(1, 86400 - rq[i].td);
		rq[i].bw = H.U_Randint(6,16);
		interval[i] = H.U_Randint(10,30);
	}
	ofstream file6("result6.txt");
	file6 << "tnum/cost\tCTLink\tArrayList\tBplus\tX" << endl;
	file6.close();
	for (unsigned int i = 4; i <= 2048; i*=2) {
		ofstream file("result6.txt", ios::app);
		file << i;
		cout << i << endl;
		//CTLink
		{
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
		file << endl;
		file.close();
	}
	//CArrayList
	{
		ofstream file("result6.txt", ios::app);
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
		ofstream file("result6.txt", ios::app);
		cout << "Bplus:" << endl;
		Bplus* bp = new Bplus();
		unsigned int t = 0;
		clock_t start = clock();
		for (unsigned int k = 0; k < REQUEST_NUM; k++) {
			rq[k].ts += t;
			bp->Insert(rq[k]);
			t += interval[k];
			if((k+1)%1000==0){
				bp->Delete(t);
			}
		}
		file << "\t" << clock() - start;
		delete bp;
		file << endl;
		file.close();
	}
}


