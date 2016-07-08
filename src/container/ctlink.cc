/*
 * ctlink.cpp
 *
 *  Created on: 2013-9-18
 *      Author: Fanjing
 */

#include "ctlink.h"
CTLink::CTLink() {
    initCTLink(256, 4096);
}
CTLink::CTLink(unsigned int tnum, unsigned int rmax) {
    // tnum means the CT_TACK_NUM; inum means the CT_INDEX_NUM; max means the CT_MAX_RESERVE_TIME.
    initCTLink(tnum, rmax);
}

CTLink::~CTLink() {
    // cout<<"~CTLink"<<endl;
    // get the first alive tack, the one before iCurrentTime
    unsigned int iStartTackNum = iCurrentTime / CT_TACK_INTERVAL;
    if(iStartTackNum < 1){
        iCurrentTackLoc = 0;
    }else{
        iCurrentTackLoc = (iStartTackNum + CT_TACK_ARRAY_SIZE - 1)
                % CT_TACK_ARRAY_SIZE;
    }
    CTNode* ptrNode = tack[iCurrentTackLoc].node->next;
    while(ptrNode){
        delete (ptrNode->pre);
        ptrNode = ptrNode->next;
    } //delete the link list
    delete (ptrNode);
    delete[] (tack); //delete the tacks
}

void CTLink::setTime(unsigned int t) {
    // maintain the current time and memory.
    if(t <= iCurrentTime){
        return;
    }
    iCurrentTime = t;
    unsigned int iCurrentTackNum = t / CT_TACK_INTERVAL;

    // the tack is 1 smaller than iCurrentTack is to store the start resource, so if a node is 2 or more smaller than iCurrentTack, it needs to be cleared.
    // Update at 1310012316: change the algorithm to fix the bug.
    for(; iStartTack + 1 < iCurrentTackNum; iStartTack++){
        cleanTack(iStartTack);
    }
    // only return true.
    return;
}

bool CTLink::insert(Request request) {
    // accept judgment.
    if(request.td == 0){
        return true;
    }

    if(!accept(request)){
        // if not accepted, return FALSE.
        return false;
    }else{
        forceInsert(request);
    }
    return true;
}

bool CTLink::accept(Request request) {
    // Warning: if r.td == 0 && r.bw + res[st] > max, this will return false;

    // update at 1309251613: this function has been changed into following function.
    // if request r can be accepted return the start node which is the first node after the start time
    // to avoid point to NULL, the tack array size should be two more than the tack number. one to record the start resource, one to record end point.
    // else if request r can not be accepted then return NULL.
    unsigned int st, et; // st stands for the start time of this request, et stands for the end time.
    CTNode* temp = NULL; // temp is used as temp node to mark the search start.
    unsigned int tackLoc; // used to store the loc of tack and index, if there is an index.
    st = iCurrentTime + request.ts;
    et = st + request.td;
    if(et > iCurrentTime + CT_MAX_RESERVE_TIME){ // request r is out of range.
        return false;
    }
    tackLoc = getTackLoc(st);
    temp = tack[tackLoc].node;
    // select start point to search. st->[temp->t,NULL)
    // select start point to judge. the result needs sp.t->[st,next), next node is the first node after sp.
    // since the judge process needs to use the node before start point, so start point is the first node after st.
    while(temp->t <= st){
        temp = temp->next;
    }
    next2st = temp; //use result to store this start point. if request r is accepted, this node will be used to create returned result node.
    // judge whether this request can be accepted.
    // since the judge process will use the data before et and the judge process just use the node before node temp
    // so the end point is the first node not before et. need ) then )+] = );
    // pseudo-code: judge: if temp->pre->rs + r.bw > iMaxResource RETURN NULL;
    // Update at 1310012249: node->rs decides the resource after node->t
    // so if the rs of which node from the last one before st to last one before et is more than r.bw, request r can be accepted.
    // update at 1310201505: change the algorithmic logic to fix a bug that temp can be pointed to NULL.
    if(temp->pre->rs + request.bw > iMaxResource){
        return false;
    }
    while(temp->t < et){
        if(temp->rs + request.bw > iMaxResource){
            return false;
        }
        temp = temp->next;
    }
    // set the next2et to the node next to end time.
    if(temp->t == et){
        temp = temp->next;
    }
    next2et = temp;
    //judge process finished and request r is accepted.
    return true;
}

CTNode* CTLink::insertNode(unsigned int t, CTNode* next) {
    // this function should have these function
    // 1. maintain the index, redirect the pointer.
    // 2. maintain the tack->num,iIndexMask, pointer and other.
    // 3. if target is a new node, insert it ,and return it. else, modify this exist node, and return it.

    CTNode* result = NULL;
    CTNode* pre = next->pre;
    unsigned int tackLoc = getTackLoc(t);
    CTTack* theTack = &tack[tackLoc];
    // situation 0: in this insert operation, if the node at time t has already exist, just return it.
    // else, insert new one, and return the new node.
    if(pre->t == t){
        // add a judgment to avoid to treat normal node as temporary node.
        if(theTack->flag == false){
            theTack->flag = true;
        }
        result = pre;
    }else{
        if(theTack->flag == false){
            // situation 1: this tack has just one dummy node. then make this dummy node a real node.
            result = pre;
            result->t = t;
            theTack->flag = true;
        }else{
            // situation 2: this tack already has at last one normal node. so insert node before loc node.
            // and after insert operation, if theTack->node == next , then change theTack->node to this new node.
            result = new CTNode();
            result->t = t;
            result->pre = pre;
            result->next = next;
            result->rs = result->pre->rs;
            pre->next = result;
            next->pre = result;
            if(theTack->node == next){
                theTack->node = result;
            }
        } // end of insert new node.
    } // end of insertion.

    return result;
}

bool CTLink::cleanTack(unsigned int n) {
    // this function will clear this tack, and link this tack to last usable tack. n stands for the tack number.
    // 1. clear all tack; 2. link this tack to the last tack.
    // Update at 1310012228: change n to n+CT_TACK_ARRAY_SIZE to avoid to mod a negative number.
    unsigned int loc = n % CT_TACK_ARRAY_SIZE;
    // update at 201503021017, remove mod operation to increase the efficiency
    unsigned int pre = loc == 0 ? CT_TACK_ARRAY_SIZE - 1 : loc - 1;
    unsigned int next = loc == CT_TACK_ARRAY_SIZE - 1 ? 0 : loc + 1;
    CTNode* nextTackNode = tack[next].node;
    // clear this tack.
    // 1st. delete the node.
    // the start node of next tack tack[next].node;
    // change the judgment statement from t < et to temp!=tack[next].node.
    // edited at 1311291157: do not delete the first node in the tack. this node will be used as the temporary tack node.
    for(CTNode* temp = tack[loc].node->next; temp!=nextTackNode;){
        temp = temp->next;
        delete (temp->pre);
    }
    // fix pre point of the node next to this tack.
    nextTackNode->pre = NULL;
    // Important!! there is a critical issue in this function. It is necessary to keep a live tack before current tack to record the start resource.
    // THIS HAS BEEN FIXED AT 1309291826.
    // 2nd. delete the index if necessary.
    // link this tack to the last tack.
    // since the last tack is used to store the end point, it has only one node which stands for the end time.
    // so just initialize this tack and link it to the the start node of previous tack.
    // 1st. initial this tack.
    tack[loc].flag = false;
    tack[loc].node->rs = 0;
    tack[loc].node->t = tack[pre].node->t + CT_TACK_INTERVAL;
    // 2nd. link this tack to previous one.
    tack[loc].node->next = NULL;
    tack[loc].node->pre = tack[pre].node; // since last tack just has one node, the first node of it is the last one.
    // 3rd. link previous tack to this one.
    tack[pre].node->next = tack[loc].node;
    // return true. there is no other result in normal.
    return true;
}

unsigned int CTLink::getTackLoc(unsigned int t) {
    //get the tack index of the time t.
    return (t / CT_TACK_INTERVAL) % CT_TACK_ARRAY_SIZE;
}

bool CTLink::Output() {
    cout << "CTLINK:DISPLAY." << endl;
    for(CTNode* temp = tack[(getTackLoc(iCurrentTime) + CT_TACK_ARRAY_SIZE - 1)
            % CT_TACK_ARRAY_SIZE].node; temp != NULL; temp = temp->next){
        cout << "rs: " << temp->t << ", " << temp->rs << endl;
    }
    return true;
}

void CTLink::initCTLink(unsigned int tnum, unsigned int max) {
    CT_TACK_NUM = tnum;
    CT_MAX_RESERVE_TIME = max; //16*4*4=256
    CT_TACK_ARRAY_SIZE = CT_TACK_NUM + 3; // one to keep all alive node in range; one to record start node; one to record end node.
    CT_TACK_INTERVAL = (CT_MAX_RESERVE_TIME + CT_TACK_NUM - 1) / CT_TACK_NUM; // to make sure that CT_TACK_INTERVAL * CT_TACK_NUM >= CT_MAX_RESERVE_TIME.
    iStartTack = 0;
    iCurrentTackLoc = 0;
    iCurrentTime = 0;
    iMaxResource = MAX;
    next2st = NULL;
    next2et = NULL;
    tack = new CTTack[CT_TACK_ARRAY_SIZE];
    // 1st. initialize all tacks and their nodes.
    for(unsigned int i = 0; i < CT_TACK_ARRAY_SIZE; i++){
        //initial each tack.
        tack[i].flag = false;
        tack[i].node = new CTNode();
        tack[i].node->t = (((i + CT_TACK_ARRAY_SIZE
                - (iStartTack % CT_TACK_ARRAY_SIZE)) % CT_TACK_ARRAY_SIZE)
                + iStartTack) * CT_TACK_INTERVAL;
        tack[i].node->rs = 0;
    }
    // 2nd. then link these nodes.
    for(unsigned int i = 0; i < CT_TACK_ARRAY_SIZE; i++){
        if((iStartTack + CT_TACK_ARRAY_SIZE) % CT_TACK_ARRAY_SIZE == i){
            //the first tack dose not have the pre tack.
            tack[i].node->pre = NULL;
            tack[i].node->next = tack[(i + 1) % CT_TACK_ARRAY_SIZE].node;
            if(iStartTack == 0){
                tack[i].flag = true; // if the current tack is the 0 tack, make the start node at time 0 to a normal node. To fix the NULL point bug.
            }
        }else if((iStartTack + CT_TACK_ARRAY_SIZE - 1) % CT_TACK_ARRAY_SIZE
                == i){
            //the last tack dose not have the next tack.
            //update at 1311011842: to fix error in ~CTLINK()
            tack[i].node->pre = tack[(i - 1) % CT_TACK_ARRAY_SIZE].node;
            tack[i].node->next = NULL;
        }else{ //the middle tacks have both pre tack and next tack.
            tack[i].node->pre = tack[(i - 1) % CT_TACK_ARRAY_SIZE].node;
            tack[i].node->next = tack[(i + 1) % CT_TACK_ARRAY_SIZE].node;
        }
    }
}

bool CTLink::forceInsert(Request request) {
    // if accepted, modify the resource of each node.
    unsigned int st = iCurrentTime + request.ts;
    unsigned int et = st + request.td;
    // select the start point.
    CTNode* start = insertNode(st, next2st);
    // find the next node to the et node.
    // update at 1312072208: change the accept function to get end time node faster.
    // insert et node.
    insertNode(et, next2et);
    // after insertion of the node et, the node before et can be modified.
    for(; start->t < et; start = start->next){
        start->rs = start->rs + request.bw;
    }
    return true;
}

void CTLink::setResourceCap(unsigned int maxResource) {
	iMaxResource = maxResource;
}
