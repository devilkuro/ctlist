/*
 * ctilink.cc
 *
 *  Created on: 2013-9-18
 *      Author: Fanjing
 */

#ifndef _CTILINK_H_
#define _CTILINK_H_

#include "common.h"

struct CTINode{
	unsigned int rs; // rs stands for the remainder resource.
	unsigned int t; // t stands for the time of this node.
	CTINode* pre; //point to pre node.
	CTINode* next; //point to next node.
};

struct CTITack{
	unsigned int num; //the number of the nodes which are fixed by this tack. Used to decide when to build the index.
	bool indexed; //be true, if the nodes after this tack have been indexed.
				  //To improve the performance, it can be replaced by several bits in one int type.
	unsigned int iIndexMask; //help maintain the index.
	CTINode* node; //point to the first node after this tack.
	CTINode** index; //this will point to the index of this tack if there is one, else this will be null.
};

class CTILink{
public:
	CTILink();
	CTILink(unsigned int tnum,unsigned int inum,unsigned int max);
	~CTILink();
public:
	// add public member variable here
	unsigned int CTI_INDEX_THRESHOLD;
	unsigned int iMaxResource; //stands for the max available resource.
public:
	bool Insert(Request r); //return true if success.
	bool SetTime(unsigned int t); //set the current time.
	bool Output(); // display the link list.
private:
	unsigned int CTI_TACK_NUM;
	unsigned int CTI_INDEX_NUM;
	unsigned int CTI_MAX_RESERVE_TIME;
	unsigned int CTI_TACK_ARRAY_SIZE;
	unsigned int CTI_TACK_INTERVAL; //decide the interval between two tacks.
	unsigned int CTI_INDEX_INTERVAL; //decide the interval between two indexes.
	CTITack* tack;
	unsigned int iStartTack; // to mark the start tack.
	unsigned int iCurrentTackLoc; //pCurrentTack stands for the tack num that include current time. current ->[tack).
	unsigned int iCurrentTime; //stands for current time.
private:
	void initCTILink(unsigned int tnum,unsigned int inum,unsigned int max);
	CTINode* insertNode(unsigned int t, CTINode* loc); //insert target node into the link list, node loc stands for the first node after target node.
	CTINode* accept(Request r); //to judge whether the request r can be accecpted or not
	bool clearTack(unsigned int n); // to clear tack n.
	unsigned int getTackLoc(unsigned int t);
	unsigned int getIndexLoc(unsigned int t);
};


#endif /* _CTILINK_H_ */
