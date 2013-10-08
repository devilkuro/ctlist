/*
 * CArrayList.cpp
 *
 *  Created on: 2013年10月8日
 *      Author: Fanjing-LAB
 */

#include "CArrayList.h"

CArrayList::CArrayList() {
	array_size = 100;
	time = 0;
	max_resource = MAX;
	resource = new unsigned int[array_size];
	memset(resource,0,array_size*sizeof(unsigned int));
}

CArrayList::CArrayList(unsigned int size) {
	array_size = size;
	time = 0;
	max_resource = MAX;
	resource = new unsigned int[array_size];
	memset(resource,0,array_size*sizeof(unsigned int));
}

unsigned int CArrayList::getIndex(unsigned int t) {
	return t%array_size;
}

CArrayList::~CArrayList() {
	delete resource;
}

bool CArrayList::Insert(Request r) {
	unsigned int st = time+r.ts;
	unsigned int et = st+r.td;
	if(accept(r)){
		for(unsigned int i = st;i<et;i++){
			resource[getIndex(i)] += r.bw;
		}
		return true;
	}else{
		return false;
	}
}

void CArrayList::setTime(unsigned int t) {
	// the nodes before time t need to be reset.
	for(;time<t;time++){
		resource[getIndex(time)] = 0;
	}
}

bool CArrayList::accept(Request r) {
	if((unsigned int)(r.ts+r.td)>array_size){
		return false;
	}else{
		unsigned int st = time+r.ts;
		unsigned int et = st+r.td;
		for(unsigned int i = st;i<et;i++){
			if(resource[getIndex(i)] + r.bw > max_resource){
				return false;
			}
		}
		return true;
	}
}


