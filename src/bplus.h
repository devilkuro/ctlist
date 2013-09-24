/*
 * bplus.h
 *
 *  Created on: 2013-9-18
 *      Author: Fanjing
 */

#ifndef _BPLUS_H_
#define _BPLUS_H_


#include "common.h"

const int m = 3;
const int s1 = 2; //��һ�����ѽ��Ĺؼ��ָ���Ҫ��
const int s2 = 2;
struct BNode
{
	int keynum;
	int key[m+1];
	int record[m+1];
	struct BNode *ptr[m+1];
	struct BNode *parent;
	int seq;
	struct BNode *next;
};

struct Result
{
	BNode *ptr;
	int i;
	int tag;
};

class Bplus
{
public:
	Bplus();
	Result* Search(int x);
	void Split(BNode *tmp);
	void Insert(Request a);
	void DSplit(BNode *tmp);
	bool Delete(int x);
	int Count();
	void Display();
	void FDisplay(int t2);
	int End();
public:
	struct BNode *root;
	struct BNode *first;
	static int sumB;
};

#endif /* _BPLUS_H_ */
