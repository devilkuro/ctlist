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
	delete[] resource;
}

bool CArrayList::Insert(Request r) {
	if(accept(r)){
		unsigned int st = time+r.ts;
		unsigned int et = st+r.td;
		unsigned int tl = getIndex(time);
		unsigned int sl = getIndex(st);
		unsigned int el = getIndex(et);
		if(sl>= tl){
			// the start local is after the time local.
			if(el>=tl){
				// the start local and the end local are both after the time local.
				// in this situation, just modify each time point one by one.
				for(;sl<el;sl++){
					resource[sl] += r.bw;
				}
			}else{
				// the start local is after the time local, but the end local is before the time local.
				// in this situation, modify each time point in two parts.
				for (; sl < array_size; sl++) {
					resource[sl] += r.bw;
				}
				for(sl = 0;sl<el; sl++){
					resource[sl] += r.bw;
				}
			}
		}else{
			// the start local is before the time local.
			// in this situation, just modify each time point from start local to end local one by one.
			for(;sl<el;sl++){
				resource[sl] += r.bw;
			}
		}
		return true;
	}else{
		return false;
	}
}

void CArrayList::setTime(unsigned int t) {
	// the nodes before time t need to be reset. (t must not smaller than time)
	if(t>=time+array_size){
		// all points need to be reset.
		memset(resource,0,array_size*sizeof(unsigned int));
		time = t;
	}else{
		unsigned int tl = getIndex(time);
		unsigned int el = getIndex(t);
		unsigned int size_uint = sizeof(unsigned int);
		if(el>tl){
			// the end local is after the time local.
			// in this situation, just reset the point from time local to end local.
			memset(resource+tl,0,(el-tl)*size_uint);
			time = t;
		}else{
			// the end local is before the time local.
			// in this situation, the reset process should be divided into two parts.
			memset(resource+tl,0,(array_size-tl)*size_uint);
			memset(resource,0,el*size_uint);
			time = t;
		}
	}
}

bool CArrayList::accept(Request r) {
	if((unsigned int)(r.ts+r.td)>array_size){
		return false;
	}else{
		unsigned int st = time+r.ts;
		unsigned int et = st+r.td;
		unsigned int tl = getIndex(time);
		unsigned int sl = getIndex(st);
		unsigned int el = getIndex(et);
		if(sl>= tl){
			// the start local is after the time local.
			if(el>=tl){
				// the start local and the end local are both after the time local.
				// in this situation, just compare each time point one by one.
				for(;sl<el;sl++){
					if(resource[sl] + r.bw > max_resource){
						return false;
					}
				}
			}else{
				// the start local is after the time local, but the end local is before the time local.
				// in this situation, compare each time point in two parts.
				for (; sl < array_size; sl++) {
					if (resource[sl] + r.bw > max_resource) {
						return false;
					}
				}
				for(sl = 0;sl<el; sl++){
					if (resource[sl] + r.bw > max_resource) {
						return false;
					}
				}
			}
		}else{
			// the start local is before the time local.
			// in this situation, just compare each time point from start local to end local one by one.
			for(;sl<el;sl++){
				if(resource[sl] + r.bw > max_resource){
					return false;
				}
			}
		}
		return true;
	}
}


