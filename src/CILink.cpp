/*
 * CILink.cpp
 *
 *  Created on: 2013-10-31
 *      Author: Fanjing
 */

#include "CILink.h"

CILink::CILink() {
	initCILink(100,MAX);
}

CILink::~CILink() {
	// free the link list
	while(head!=NULL){
		CINode* temp = head;
		head = head->next;
		delete temp;
	}
	// free the index array
	delete[] index;
}

bool CILink::Insert(Request r) {
	// TODO fix this function
	// accept judgment.
	if(r.td == 0){
		return true;
	}

	CINode* pre2st = NULL;
	CINode* pre2et = NULL;
	pre2st = accept(r);
	if(pre2st == NULL){
		return false;
	}else{
 		unsigned int st = iCurrentTime + r.ts;
 		unsigned int et = st + r.td;

 		CINode* start = insertNode(st,pre2st);
 		// TODO: fix this function.
	}
	return false;
}

bool CILink::Output() {
	// TODO fix this function

	return false;
}

void CILink::initCILink(unsigned int inum, unsigned int rmax) {
	iMaxResource = MAX;
	CI_INDEX_NUM = inum;
	CI_INDEX_ARRAY_SIZE = CI_INDEX_NUM + 2;
	CI_MAX_RESERVE_TIME = rmax;
	CI_INDEX_INTERVAL = (rmax + CI_INDEX_NUM - 1)/CI_INDEX_NUM;
	iCurrentIndex = 0;
	index = new CIndex[CI_INDEX_NUM];
	for(unsigned int i = 0;i<CI_INDEX_NUM;i++){
		index[i].node = NULL;
	}
	// initialize the head node.
	head = new CINode();
	head->t = 0;
	head->rs = 0;
	head->pre = NULL;
	head->next = NULL;
	index[0] = head;
}

CINode* CILink::insertNode(unsigned int t, CINode* pre) {
	CINode* result = NULL;
	if(pre->t == t){
		result = pre;
	}else{
		// creat a new node if there is not.
		result = new CINode();
		result->t = t;
		result->rs = pre->rs;
		// link these nodes.
		if(pre->next!=NULL){
			pre->next->pre = result;
		}
		result->next = pre->next;
		result->pre = pre;
		pre->next = result;
		// update the index
		unsigned int resultIndexLoc = getIndexLoc(t);
		unsigned int preIndexLoc = getIndexLoc(pre->t);
		if(preIndexLoc!=resultIndexLoc){
			index[resultIndexLoc] = result;
		}
	}
	return result;
}

CINode* CILink::accept(Request r) {
	unsigned int st, et;
	CINode* result;
	CINode* temp;
	unsigned int indexLoc;

	st = iCurrentTime + r.ts;
	et = st + r.td;
	// TODO: need to be confirmed twice. > or >=
	if( et>iCurrentTime+CI_MAX_RESERVE_TIME){
		return NULL;
	}
	// find the first usable index.
	indexLoc = getIndexLoc(st);
	while(index[indexLoc]==NULL){
		// move index location forward.
		if(indexLoc>0){
			indexLoc--;
		}else{
			indexLoc = CI_INDEX_ARRAY_SIZE-1;
		}
	}
	// find the last node which is not after the start time.
	temp = index[indexLoc];
	while(temp->next!=NULL){
		if(temp->next->t>st){
			break;
		}
		temp = temp->next;
	}
	result = temp;
	// decide to accept or not.
	while(temp->next!=NULL){
		// if the resource if not enough, return NULL.
		if(temp->rs+r.bw>iMaxResource){
			return NULL;
		}
		// if the next node is after the end time, break out.
		if(temp->next->t>=et){
			break;
		}
	}

	return result;
}

bool CILink::SetTime(unsigned int t) {
	if(t<=iCurrentTime){
		return false;
	}
	iCurrentTime = t;
	unsigned int iCurrentIndexNum = iCurrentTime/CI_INDEX_INTERVAL;
	for(;iStartIndex+1<iCurrentIndexNum; iStartIndex++){
		clearIndex(iStartIndex);
	}
	return true;
}

// clear the index that unused. n means the index number which will be cleared.
bool CILink::clearIndex(unsigned int n) {
	// 0th. get the index location
	unsigned int loc = n%CI_INDEX_ARRAY_SIZE;
	unsigned int next = (n+1)%CI_INDEX_ARRAY_SIZE;
	// 1st. reset the index
	this->index[loc] = NULL;
	// 2nd. free the nodes during index n.
	// get the end time
	unsigned int et = (n+1)*CI_INDEX_INTERVAL;
	// find the last node before end time
	CINode* lastNode = NULL;
	for(lastNode = head;lastNode->next!=NULL;lastNode = lastNode->next){
		// is lastNode is the last node before end time, break out.
		if(lastNode->next->t>=et){
			// delete the pre node link of the lastNode, since there is no pre node anymore.
			lastNode->pre = NULL;
			break;
		}
		CINode* temp = lastNode;
		delete temp;
	}
	// at this step, the lastNode is the last node before end time.
	// Then if the head node is not the last node, replace the head with lastNode and delete the original head node.
	if(lastNode!=head){
		delete head;
		head = lastNode;
	}
	// 3rd. link head to the next index
	this->index[next] = head;
	// return true. there is not any other result.
	return true;
}

unsigned int CILink::getIndexLoc(unsigned int t) {
	return (t/CI_INDEX_INTERVAL)%CI_INDEX_ARRAY_SIZE;
}
