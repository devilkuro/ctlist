/*
 * ctlink.cc
 *
 *  Created on: 2013-9-18
 *      Author: Fanjing
 */

#ifndef _CTLINK_H_
#define _CTLINK_H_

#include "common.h"
#include "BaseAdmissionController.h"

struct CTNode {
    unsigned int rs; // rs stands for the remainder resource.
    unsigned int t; // t stands for the time of this node.
    CTNode* pre; //point to pre node.
    CTNode* next; //point to next node.
};

struct CTTack {
    bool flag; //the number of the nodes which are fixed by this tack. Used to decide when to build the index.
//	bool indexed; //be true, if the nodes after this tack have been indexed.
    //To improve the performance, it can be replaced by several bits in one int type.
//	unsigned int iIndexMask; //help maintain the index.
    CTNode* node; //point to the first node after this tack.
//	CTNode** index; //this will point to the index of this tack if there is one, else this will be null.
};

class CTLink : public BaseAdmissionController {
public:
    CTLink();
    CTLink(unsigned int tnum, unsigned int rmax);
    ~CTLink();
public:
    // add public member variable here
    unsigned int iMaxResource; //stands for the max available resource.
public:
    virtual bool insert(Request request); //return true if success.
    virtual void setTime(unsigned int t); //set the current time.
    virtual bool accept(Request request); //to judge whether the request r can be accecpted or not
    virtual bool forceInsert(Request request);
    virtual bool Output(); // display the link list.
private:
    unsigned int CT_TACK_NUM;
    unsigned int CT_MAX_RESERVE_TIME;
    unsigned int CT_TACK_ARRAY_SIZE;
    unsigned int CT_TACK_INTERVAL; //decide the interval between two tacks.
    CTTack* tack;
    unsigned int iStartTack; // to mark the start tack.
    unsigned int iCurrentTackLoc; //pCurrentTack stands for the tack num that include current time. current ->[tack).
    unsigned int iCurrentTime; //stands for current time.
private:
    void initCTLink(unsigned int tnum, unsigned int max);
    CTNode* insertNode(unsigned int t, CTNode* loc); //insert target node into the link list, node loc stands for the first node after target node.
    bool cleanTack(unsigned int n); // to clear tack n.
    unsigned int getTackLoc(unsigned int t);

    CTNode* next2st;
    CTNode* next2et;
};

#endif /* _CTLINK_H_ */
