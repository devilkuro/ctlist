/*
 * ctlink.cc
 *
 *  Created on: 2013-9-18
 *      Author: Fanjing
 */

#ifndef _CTLINK_H_
#define _CTLINK_H_

#include "common.h"

#define CT_TACK_NUM  16
#define CT_INDEX_NUM 4
#define CT_MAX_RESERVE_TIME 256 //16*4*4=256
struct CTNode{
	unsigned int rs; // rs stands for the remainder resource.
	unsigned int t; // t stands for the time of this node.
	CTNode* pre; //point to pre node.
	CTNode* next; //point to next node.
};

struct CTTack{
	unsigned int num; //the number of the nodes which are fixed by this tack. Used to decide when to build the index.
	bool indexed; //be true, if the nodes after this tack have been indexed.
				  //To improve the performance, it can be replaced by several bits in one int type.
	unsigned int iIndexMask; //help maintain the index.
	CTNode* node; //point to the first node after this tack.
	CTNode** index; //this will point to the index of this tack if there is one, else this will be null.
};

class CTLink{
public:
	CTLink();
	~CTLink();
public:
	// add public member variable here
	unsigned int CT_INDEX_THRESHOLD;
	unsigned int CT_TACK_ARRAY_SIZE;
	unsigned int CT_TACK_INTERVAL; //decide the interval between two tacks.
	unsigned int CT_INDEX_INTERVAL; //decide the interval between two indexes.
public:
	bool Insert(Request r); //return true if success.
	bool SetTime(unsigned int t); //set the current time.
	bool Output(); // display the link list.
private:

	CTTack* tack;
	unsigned int iStartTack; // to mark the start tack.
	unsigned int iCurrentTack; //pCurrentTack stands for the tack num that include current time. current ->[tack).
	unsigned int iCurrentTime; //stands for current time.
	unsigned int iMaxResource; //stands for the max available resource.
private:
	CTNode* insertNode(unsigned int t, CTNode* loc); //insert target node into the link list, node loc stands for the first node after target node.
	CTNode* accept(Request r); //to judge whether the request r can be accecpted or not
	bool clearTack(unsigned int n); // to clear tack n.
	unsigned int getTackLoc(unsigned int t);
	unsigned int getIndexLoc(unsigned int t);
};


#endif /* _CTLINK_H_ */
