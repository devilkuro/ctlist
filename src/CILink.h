/*
 * CILink.h
 *
 *  Created on: 2013-10-31
 *      Author: Fanjing
 */

#ifndef _CILINK_H_
#define _CILINK_H_

#include "common.h"
#include "BaseAdmissionController.h"

struct CINode {
    unsigned int rs; // rs stands for the remainder resource after time t.
    unsigned int t; // t stands for the time of this node.
    CINode* pre; //point to pre node.
    CINode* next; //point to next node.
};
struct CIndex {
    unsigned int flag;
    CINode* node; //point to the first node after this tack.
};
/*
 *
 */
class CILink:public BaseAdmissionController {
public:
    CILink();
    CILink(unsigned int inum, unsigned int rmax);
    virtual ~CILink();
public:
    // add public member variable here
    unsigned int iMaxResource; //stands for the max available resource.
public:
    virtual void setTime(unsigned int t); //set the current time.
    virtual bool accept(Request r); //to judge whether the request r can be accecpted or not
    virtual bool forceInsert(Request r);
    virtual bool insert(Request r); //return true if success.
    virtual bool Output(); // display the link list.
private:
    // add private member variable here
    // the number of usable index
    unsigned int CI_INDEX_NUM;
    // the size of index[], equals to 2 added to CI_INDEX_NUM
    unsigned int CI_INDEX_ARRAY_SIZE;
    // the reserve time limit
    unsigned int CI_MAX_RESERVE_TIME;
    // the interval between two index
    unsigned int CI_INDEX_INTERVAL;
    CIndex* cIndex;
    CINode* head;
    // the number of the index which contains the current time
    unsigned int iCurrentIndex;
    // the first index
    unsigned int iStartIndex;
    // the current time
    unsigned int iCurrentTime;

private:
    // add private function here
    void initCILink(unsigned int inum, unsigned int rmax);
    CINode* insertNode(unsigned int t, CINode* loc); //insert target node into the link list, node loc stands for the first node before time t.
    bool cleanIndex(unsigned int n); // to clear index n.
    unsigned int getIndexLoc(unsigned int t);
    CINode* pre2st;
    CINode* pre2et;
};

#endif /* _CILINK_H_ */
