/*
 * ctlink.cpp
 *
 *  Created on: 2013-9-18
 *      Author: Fanjing
 */

#include "ctlink.h"

unsigned int CTLink::getTackLoc(unsigned int t){
	//get the tack index of the time t.
	return (t/CT_TACK_INTERVAL)%CT_TACK_ARRAY_SIZE;
}

unsigned int CTLink::getIndexLoc(unsigned int t){
	//get the index num of the time t.
	return (t%CT_TACK_INTERVAL)/CT_INDEX_INTERVAL;
}

CTNode* CTLink::insertNode(unsigned int t, CTNode* loc) {
	// TODO fix this function. 2013-9-25
	// this function should have these function
	// 1. maintain the index, redirect the pointer.
	// 2. maintain the tack->num,iIndexMask, pointer and other.
	// 3. if target is a new node, insert it ,and return it. else, modify this exist node, and return it.

	CTNode* result;
	CTNode* pre = loc->pre;
	unsigned int tackLoc = getTackLoc(t);
	if(tack[tackLoc].num == 0){
		// situation 1: this tack has just one temporary node. then replace this temporary node with target node.
		result = pre;
		result->t = t;
		tack[tackLoc].num++;
	}else if(tack[tackLoc].num < CT_INDEX_THRESHOLD){
		//TODO fix this part. 2013-9-25
		// situation 2: this tack already has at last one normal node. so insert node before loc node.
		// in this insert operation, if the node at time t has already exist, just return it.
		// else, insert new one, and return the new node.
		// and after insert operation, if tack[tackLoc].node == loc , then change tack[tackLoc].node to this new node.
		if(pre->t == t){
			result = pre;
		}else {
			result = new CTNode();
			result->t = t;
			result->pre = pre;
			result->next = loc;
			pre->next = result;
			loc->pre = result;
			if(tack[tackLoc].node == loc){
				tack[tackLoc].node = result;
			}
			tack[tackLoc].num++;
		}
	}else if (tack[tackLoc].num == CT_INDEX_THRESHOLD){
		//
	}

	return NULL;
}

CTNode* CTLink::accept(Request r) {
	// update at 1309251613: this function has been changed into following function.
	// if request r can be accepted return the start node which is the first node after the start time
	// to avoid point to NULL, the tack array size should be two more than the tack number. one to record the start resource, one to record end point.
	// else if request r can not be accepted then return NULL.
	unsigned int st,et; // st stands for the start time of this request, et stands for the end time.
	CTNode* result,* temp; //result is used to store the result node. temp is used as temp node to mark the search start.
	unsigned int tackLoc, indexLoc; //used to store the loc of tack and index, if there is an index.
	st = iCurrentTime + r.ts;
	et = st+r.td;

	tackLoc = getTackLoc(st);
	if(et>iCurrentTime+CT_MAX_RESERVE_TIME) //request r is out of range.
		return NULL;
	if(tack[getTackLoc(st)].indexed){
		indexLoc = getIndexLoc(st);
		temp=tack[tackLoc].index[indexLoc];
	}else{
		temp=tack[tackLoc].node;
	} //select start point to search. st->[temp->t,NULL)
	// select start point to judge. the result needs sp.t->[st,next), next node is the first node after sp.
	// since the judge process needs to use the node before start point, so start point is the first node after st.
	while(temp->t<=st){
		temp=temp->next;
	}
	result = temp; //use result to store this start point. if request r is accepted, this node will be used to create returned result node.
	// judge whether this request can be accepted.
	// since the judge process will use the data before et and the judge process just use the node before node temp
	// so the end point is the first node not before et. need ) then )+] = );
	// pseudo-code: judge: if temp->pre->rs + r.bw > iMaxResource RETURN NULL;
	while(temp->t<et){
		if(temp->pre->rs + r.bw > iMaxResource){
			return NULL;
		}
		temp=temp->next;
	}
	//judge process finished and request r is accepted.
	return result;
}

CTLink::CTLink() {
	CT_TACK_NUM = 64;
	CT_INDEX_NUM = 8;
	CT_INDEX_THRESHOLD = (unsigned int) log2(CT_TACK_NUM);
	CT_MAX_RESERVE_TIME = 4096; //64*8*8=4096
	CT_TACK_ARRAY_SIZE = CT_TACK_NUM+2;
	CT_TACK_INTERVAL = (CT_MAX_RESERVE_TIME+CT_TACK_NUM-1)/CT_TACK_NUM;
	CT_INDEX_INTERVAL = (CT_TACK_INTERVAL+CT_INDEX_NUM-1)/CT_INDEX_NUM;
	iCurrentTack = 0;
	iCurrentTime = 0;
	iMaxResource = MAX;
	tack = new CTTack[CT_TACK_ARRAY_SIZE];
	for (unsigned int i = 0; i < CT_TACK_ARRAY_SIZE; i++) {
		//initial each tack.
		tack[i].num = 0;
		tack[i].indexed = false; //new tack has no index
		tack[i].iIndexMask = 0;
		tack[i].index = NULL;
		tack[i].node = new CTNode();
		if (iCurrentTack % CT_TACK_ARRAY_SIZE == i) { //the first tack dose not have the pre tack.
			tack[i].node->pre = NULL;
			tack[i].node->next = tack[(i + 1) % CT_TACK_ARRAY_SIZE].node;
		} else if (iCurrentTack % CT_TACK_ARRAY_SIZE - 1 == i) { //the last tack dose not have the next tack.
			tack[i].node->pre = tack[(i - 1) % CT_TACK_ARRAY_SIZE].node;
			tack[i].node->next = NULL;
		} else { //the middle tacks have both pre tack and next tack.
			tack[i].node->pre = tack[(i - 1) % CT_TACK_ARRAY_SIZE].node;
			tack[i].node->next = tack[(i + 1) % CT_TACK_ARRAY_SIZE].node;
		}
		tack[i].node->t = ((i+CT_TACK_ARRAY_SIZE-iCurrentTack)%CT_TACK_ARRAY_SIZE)*CT_TACK_INTERVAL;
		tack[i].node->rs = 0;
	}
}

CTLink::~CTLink() {
	CTNode* ptrNode = tack[iCurrentTack].node->next;
	while (!ptrNode) {
		delete (ptrNode->pre);
		ptrNode = ptrNode->next;
	} //delete the link list
	delete (ptrNode);
	for (unsigned int i = 0; i < CT_TACK_ARRAY_SIZE; i++) {
		if (tack[i].indexed) {
			for (unsigned int j = 0; j < CT_INDEX_NUM; j++) {
				delete (tack[i].index[j]);
			}
		}
	} //delete all indexes
	delete (tack); //delete the tacks
}

bool CTLink::Insert(Request r) {
	//TODO fix this function. 2013-9-21

	return false;
}

bool CTLink::SetTime(unsigned int t) {
	// TODO fix this function. 2013-9-24
	//manage the current time and maintain memory.
	return false;
}


