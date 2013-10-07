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
const int s1 = 2; //第一个分裂结点的关键字个数要多
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
	Result Search(int x);
	void Split(BNode *tmp);
	bool Insert(Request a);
	void DSplit(BNode *tmp);
	bool Delete(int x);
	int Count();
	void Display();
	void FDisplay(int t2);
	int End();
	bool Output(); // display the resource link list.
public:
	struct BNode *root;
	struct BNode *first;
	static int sumB;
private:
	void deleteBNode(BNode* b);
	void deleteResult(Result* r);
};

#endif /* _BPLUS_H_ */
