/*
 * bplus.h
 *
 *  Created on: 2013-9-18
 *      Author: Fanjing
 */

#ifndef _BPLUS_H_
#define _BPLUS_H_


#include "common.h"
#include "SmartPtr.h"

const int m = 3;
const int s1 = 2; //第一个分裂结点的关键字个数要多
const int s2 = 2;
class BNode
{
public:
	int keynum;
	int key[m+1];
	int record[m+1];
	SmartPtr<BNode> ptr[m+1];
	SmartPtr<BNode> parent;
	int seq;
	SmartPtr<BNode> next;
};

class Result
{
public:
	SmartPtr<BNode> ptr;
	int i;
	int tag;
	~Result(){
		cout<<"delete Result."<<endl;
	}
};

class Bplus
{
public:
	Bplus();
	SmartPtr<Result> Search(int x);
	void Split(SmartPtr<BNode> tmp);
	bool Insert(Request a);
	void DSplit(SmartPtr<BNode> tmp);
	bool Delete(int x);
	int Count();
	void Display();
	void FDisplay(int t2);
	int End();
	bool Output(); // display the resource link list.
public:
	SmartPtr<BNode> root;
	SmartPtr<BNode> first;
	static int sumB;
};

#endif /* _BPLUS_H_ */
