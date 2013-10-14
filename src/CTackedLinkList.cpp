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

#define CT_TEST_TIME 4305
//#define CT_DEBUG
//#define CT_DEBUG_C
//#define CT_DEBUG_B
//#define CT_DEBUG_A
//#define CT_DEBUG_B_C
#define GN_DEBUG
//#define GN_OUT_DEBUG false
//#define TEST_DEBUG
template <class T> string m_toStr(T tmp)
{
    stringstream ss;
    ss << tmp;
    return ss.str();
}
void setGN(Generator * gn, unsigned int interval, unsigned int ts_down, unsigned int ts_up,
		unsigned int td_avg, unsigned int td_limit) {
	unsigned int bw_down = 1;
	unsigned int bw_up = (2*MAX*interval)/td_avg;
	if(bw_up<1){
		bw_up = 1;
	}
	gn->setGenerator(bw_down,bw_up,ts_down,ts_up,td_avg,td_limit,interval);
}
string getFN(unsigned int interval, unsigned int ts_down, unsigned int ts_up,
		unsigned int td_avg, unsigned int td_limit) {
	return "output_i" + m_toStr(interval) + "_ts" + m_toStr(ts_down) + "_"
			+ m_toStr(ts_up) + "_td" + m_toStr(td_avg) + "_" + m_toStr(td_limit)
			+ ".dat";
}

#ifdef TEST_DEBUG
int main(){
	Helper H;
	unsigned int max = 1000000;
	unsigned int num = 0;
	unsigned int sum = 0;
	unsigned int p = 0;
	unsigned int a[101] = {0};
	for(unsigned int i = 0; i < max ; i++){
		p =  H.E_Rand(0.01);
		if(p<1010){
		a[p/10]++;
		}else{
			a[100]++;
		}
		sum += (unsigned int )p;
		if(p >= 1010){
			cout << num++ <<"\tP:"<< p << "\tavg:" << (double)sum/(i+1)<< endl;
		}
	}
	for(unsigned int i = 0;i<101;i++){
		if(i%10 == 0){
			cout<< endl;
		}
		cout<< a[i] << "\t";
	}
}
#endif
#ifdef GN_DEBUG
int main() {
#ifdef GN_OUT_DEBUG
	unsigned int size; //size of each block.
	char* buff;
	unsigned long length;//total size
	Request* rq;
	double* interval;
	char* z;
	clock_t t_start, t_write, t_read, t_total;

	t_start = clock();
#endif
	Generator* gn = new Generator();
	string fileName("output.dat");
//	gn->output(fileName.c_str(), MAX_REQUEST_NUM);
	unsigned int interval = 1;
	unsigned int ts_down = 1;
	unsigned int ts_up = 0;
	unsigned int td_avg = 0;
	unsigned int td_limit = 0;
	for(unsigned int i = 0;i<3;i++,interval*=10){
		ts_down = 1;
		for(unsigned int j = 0;j<3;j++,ts_down*=10){
			for(unsigned int k = 0;k<5;k++){
				ts_up = 3*ts_down;
				td_avg = 2*ts_down*(k+1);
				td_limit = 5*td_avg;
				setGN(gn, interval, ts_down, ts_up, td_avg, td_limit);
				fileName = getFN(interval, ts_down, ts_up, td_avg, td_limit);
				cout<< "fileName:" << fileName << endl;
				gn->output(fileName.c_str(),MAX_REQUEST_NUM/4);
			}
		}
	}
#ifdef GN_OUT_DEBUG
	t_write = clock() - t_start;

	ifstream file("output.dat");
	file.seekg(0, file.end);
	length = file.tellg();
	file.seekg(0, file.beg);

	size = sizeof(double) + sizeof(Request);
	buff = new char[length];
	file.read(buff, length);

	t_read = clock() - t_start - t_write;

	for (unsigned int i = 0; i < length / size; i++) {
		interval = (double*) (buff + i * size);
		rq = (Request*) (buff + i * size + sizeof(double));
		if (i % 100000 == 0 && GN_OUT_DEBUG) {
			cout << "read:: Request(" << rq->bw << " , " << rq->ts << " , "
			<< rq->td << " ):" << *interval << "+" << *z + 0 << endl;
		}
	}
	file.close();

	delete(buff);
	t_total = clock() - t_start;
	cout << "function started at " << t_start << endl;
	cout << "generate and write finished in " << t_write
	<< " ms and read finished in " << t_read << " ms." << endl;
	cout << "function finished in " << t_total << " ms." << endl;
#endif

	delete(gn);
	return 0;
}
#endif
#ifdef CT_DEBUG
int main() {

	Request *xlist;
	unsigned int *tlist;
	Request x;
	int t1;
	int sum = 0;
	CTLink C;
	Bplus B;
	CArrayList A = CArrayList(4096);
	int nowtime;
	clock_t ps, start;
	unsigned int startTime = clock();
	unsigned int tempTime = 0;

	xlist = new Request[MAX_REQUEST_NUM];
	tlist = new unsigned int[MAX_REQUEST_NUM];
	cout << "start to read the data." << endl;
	unsigned int size; //size of each block.
	char* buff;
	unsigned long length;//total size
	Request* rq;
	double* interval;
	ifstream file("output.dat");
	file.seekg(0, file.end);
	length = file.tellg();
	file.seekg(0, file.beg);
	size = sizeof(double) + sizeof(Request);
	buff = new char[length];
	file.read(buff, length);
	if(MAX_REQUEST_NUM!=length/size){
		cout<<"ERROR!"<<endl;
		return 0;
	}
	for (unsigned int i = 0; i < MAX_REQUEST_NUM; i++) {
		interval = (double*) (buff + i * size);
		rq = (Request*) (buff + i * size + sizeof(double));
		xlist[i].bw = rq->bw;
		xlist[i].td = rq->td;
		xlist[i].ts = rq->ts;
		tlist[i] = (unsigned int )*interval;
	}
	tempTime = clock();
	cout << "reading finished." <<endl;
	cout << "reading takes " <<(tempTime-startTime)/1000.0<<" s, "<< MAX_REQUEST_NUM <<" requests have been read."<< endl;

	tempTime = clock();

	unsigned int staInterval = 1000000;
	unsigned int acceptNum = 0;
	ps = 0;
	start = 0;//ms
#ifdef CT_DEBUG_A
	cout<< "CT_DEBUG_A::"<< endl;
#endif
#ifdef CT_DEBUG_B
	cout<< "CT_DEBUG_B::"<< endl;
#endif
#ifdef CT_DEBUG_C
	cout<< "CT_DEBUG_C::"<< endl;
#endif
	for(unsigned int i = 0;i<MAX_REQUEST_NUM;i++)
	{
		sum++;
		t1 = tlist[i];
		start = start+t1;//ms
		x.bw = xlist[i].bw;
		x.td = xlist[i].td;
		x.ts = xlist[i].ts;
		if(x.td<0){
			cout<<"Warning!"<< endl;
		}
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
#ifdef CT_DEBUG_A
		A.setTime(nowtime);
		bool flagA = A.Insert(x);
#ifndef CT_DEBUG_C
		if(flagA){
			acceptNum++;
		}
#endif
#ifdef CT_DEBUG_C
		if(flagA!=flagC){
			C.Output();
			cout<< "Error!!"<< endl;
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
#endif
