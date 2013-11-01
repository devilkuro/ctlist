/*
 * CArrayList.h
 *
 *  Created on: 2013年10月8日
 *      Author: Fanjing-LAB
 */

#ifndef _CARRAYLIST_H_
#define _CARRAYLIST_H_

#include "common.h"

class CArrayList {
public:
	CArrayList();
	CArrayList(unsigned int size);
	virtual ~CArrayList();
public:
	bool Insert(Request r);
	void setTime(unsigned int t);
private:
	bool accept(Request r);
	unsigned int getIndex(unsigned int t);
public:
	unsigned int max_resource;
private:
	unsigned int time;
	unsigned int *resource;
	unsigned int array_size;
};

#endif /* _CARRAYLIST_H_ */
