/*
 * CArrayList.cpp
 *
 *  Created on: 2013年10月8日
 *      Author: Fanjing-LAB
 */

#include "CArrayList.h"

CArrayList::CArrayList() {
	m_scale = 1;
	m_array_size = 100;
	m_time = 0;
	max_resource = MAX;
	m_resource = new unsigned int[m_array_size];
	memset(m_resource, 0, m_array_size * sizeof(unsigned int));
}

CArrayList::CArrayList(unsigned int size, unsigned int scale) {
	this->m_array_size = size;
	this->m_scale = scale;
	this->m_time = 0;
	this->max_resource = MAX;
	this->m_resource = new unsigned int[m_array_size];
	memset(m_resource, 0, m_array_size * sizeof(unsigned int));
}

bool CArrayList::forceInsert(Request request) {
	unsigned int st = m_time + request.ts;		// start time
	unsigned int et = st + request.td;			// end time
	unsigned int tl = getIndex(m_time);			// current time location
	unsigned int sl = getIndex(st);				// start time location
	unsigned int el = getIndex(et + m_scale - 1);	// end time location
	if (sl >= tl) {
		// the start local is after the time local.
		if (el >= tl) {
			// the start local and the end local are both after the time local.
			// in this situation, just modify each time point one by one.
			for (; sl < el; sl++) {
				m_resource[sl] += request.bw;
			}
		} else {
			// the start local is after the time local, but the end local is before the time local.
			// in this situation, modify each time point in two parts.
			for (; sl < m_array_size; sl++) {
				m_resource[sl] += request.bw;
			}
			for (sl = 0; sl < el; sl++) {
				m_resource[sl] += request.bw;
			}
		}
	} else {
		// the start local is before the time local.
		// in this situation, just modify each time point from start local to end local one by one.
		for (; sl < el; sl++) {
			m_resource[sl] += request.bw;
		}
	}
	return true;
}

inline unsigned int CArrayList::getIndex(unsigned int t) {
	return (t / m_scale) % m_array_size;
}

CArrayList::~CArrayList() {
	delete[] m_resource;
}

bool CArrayList::insert(Request request) {
	if (accept(request)) {
		forceInsert(request);
		return true;
	} else {
		return false;
	}
}

void CArrayList::setTime(unsigned int time) {
	// the nodes before time t need to be reset. (t must not smaller than time)
	if (time >= m_time + m_array_size * m_scale) {
		// all points need to be reset.
		memset(m_resource, 0, m_array_size * sizeof(unsigned int));
		m_time = time;
	} else {
		unsigned int tl = getIndex(m_time);
		unsigned int el = getIndex(time);
		unsigned int size_uint = sizeof(unsigned int);
		if (el > tl) {
			// the end local is after the time local.
			// in this situation, just reset the point from time local to end local.
			memset(m_resource + tl, 0, (el - tl) * size_uint);
			m_time = time;
		} else {
			// the end local is before the time local.
			// in this situation, the reset process should be divided into two parts.
			memset(m_resource + tl, 0, (m_array_size - tl) * size_uint);
			memset(m_resource, 0, el * size_uint);
			m_time = time;
		}
	}
}

bool CArrayList::accept(Request request) {
	if ((unsigned int) (request.ts + request.td) > m_array_size) {
		return false;
	} else {
		unsigned int st = m_time + request.ts;
		unsigned int et = st + request.td;
		unsigned int tl = getIndex(m_time);
		unsigned int sl = getIndex(st);
		unsigned int el = getIndex(et);
		if (sl >= tl) {
			// the start local is after the time local.
			if (el >= tl) {
				// the start local and the end local are both after the time local.
				// in this situation, just compare each time point one by one.
				for (; sl < el; sl++) {
					if (m_resource[sl] + request.bw > max_resource) {
						return false;
					}
				}
			} else {
				// the start local is after the time local, but the end local is before the time local.
				// in this situation, compare each time point in two parts.
				for (; sl < m_array_size; sl++) {
					if (m_resource[sl] + request.bw > max_resource) {
						return false;
					}
				}
				for (sl = 0; sl < el; sl++) {
					if (m_resource[sl] + request.bw > max_resource) {
						return false;
					}
				}
			}
		} else {
			// the start local is before the time local.
			// in this situation, just compare each time point from start local to end local one by one.
			for (; sl < el; sl++) {
				if (m_resource[sl] + request.bw > max_resource) {
					return false;
				}
			}
		}
		return true;
	}
}

