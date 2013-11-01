/*
 * CILink.cpp
 *
 *  Created on: 2013-10-31
 *      Author: Fanjing
 */

#include "CILink.h"

CILink::CILink() {
	// TODO Auto-generated constructor stub
	initCILink(100,MAX);
}

CILink::~CILink() {
	// TODO Auto-generated destructor stub

}

bool CILink::Insert(Request r) {
	// TODO fix this function
	return false;
}

bool CILink::SetTime(unsigned int t) {
	// TODO fix this function
	return false;
}

bool CILink::Output() {
	// TODO fix this function
	return false;
}

void CILink::initCILink(unsigned int inum, unsigned int rmax) {
	CI_INDEX_NUM = inum;
	CI_MAX_RESERVE_TIME = rmax;
	iCurrentIndex = 0;
	index = new CIndex[CI_INDEX_NUM];
	for(unsigned int i = 0;i<CI_INDEX_NUM;i++){
		index[i].node = NULL;
	}
	// TODO: ehh...fix these functions. 2013-10-31

}

CINode* CILink::insertNode(unsigned int t, CINode* loc) {
	// TODO fix this function
	return NULL;
}

CINode* CILink::accept(Request r) {
	// TODO fix this function
	return NULL;
}

bool CILink::clearIndex(unsigned int n) {
	// TODO fix this function
	return false;
}

unsigned int CILink::getIndexLoc(unsigned int t) {
	// TODO fix this function
}
