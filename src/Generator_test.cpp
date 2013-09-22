/*
 * Generator_test.cpp
 *
 *  Created on: 2013-9-20
 *      Author: Fanjing
 */

#include "Generator.h"

int main(){
	Generator* gn = new Generator();
	string fileName("output.dat");
	gn->output(fileName.c_str(),MAX_REQUEST_NUM);
	ifstream file("output.dat");
	file.seekg(0,file.end);
	unsigned long length = file.tellg();
	file.seekg (0, file.beg);
	unsigned int size = (sizeof(double)+sizeof(Request)+sizeof(char));
	char* buff = new char[length];
	file.read(buff,length);
	if(length!=size*MAX_REQUEST_NUM){
		cout<<"ERROR!"<<endl;
		file.close();
		return 0;
	}else{
		Request* rq;
		double* interval;
		char* z;
		for(unsigned int i=0;i<MAX_REQUEST_NUM;i++){
			interval = (double*)(buff+i*size);
			rq = (Request*)(buff+i*size+sizeof(double));
			z = (char*)(buff+i*size+sizeof(double)+sizeof(Request));
			if(i%1000==0){
				cout<<"read:: Request("<<rq->bw<<" , "<<rq->ts<<" , "<<rq->td<<" ):"<<*interval<<"+"<<*z+0<<endl;
			}
		}
		file.close();
	}
	return 0;
}
