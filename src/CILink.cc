/*
 * CILink.cpp
 *
 *  Created on: 2013-10-31
 *      Author: Fanjing
 */

#include "CILink.h"

CILink::CILink() {
    initCILink(100, MAX);
}
CILink::CILink(unsigned int inum, unsigned int rmax) {
    initCILink(inum, rmax);
}

CILink::~CILink() {
    // free the link list
    while(head != NULL){
        CINode* temp = head;
        head = head->next;
        delete temp;
    }
    // free the index array
    delete[] (cIndex);
}

bool CILink::Insert(Request r) {
    // accept judgment.
    if(r.td == 0){
        return true;
    }

    CINode* pre2st = NULL;
    CINode* pre2et = NULL;
    if(!accept(r, pre2st, pre2et)){
        return false;
    }else{
        unsigned int st = iCurrentTime + r.ts;
        unsigned int et = st + r.td;

        CINode* start = insertNode(st, pre2st);
        if(pre2et->t < start->t){
            insertNode(et, start);
        }else{
            insertNode(et, pre2et);
        }
        for(; start->t < et; start = start->next){
            start->rs = start->rs + r.bw;
        }
    }
    return true;
}

bool CILink::Output() {
    cout << "CILINK:DISPLAY." << endl;
    for(CINode* temp = head; temp != NULL; temp = temp->next){
        cout << "rs: " << temp->t << ", " << temp->rs << endl;
    }
    return true;
}

void CILink::initCILink(unsigned int inum, unsigned int rmax) {
    iMaxResource = MAX;
    CI_INDEX_NUM = inum;
    CI_INDEX_ARRAY_SIZE = CI_INDEX_NUM + 2;
    CI_MAX_RESERVE_TIME = rmax;
    CI_INDEX_INTERVAL = (rmax + CI_INDEX_NUM - 1) / CI_INDEX_NUM;
    iCurrentIndex = 0;
    iCurrentTime = 0;
    iStartIndex = 0;
    cIndex = new CIndex[CI_INDEX_ARRAY_SIZE];
    for(unsigned int i = 0; i < CI_INDEX_ARRAY_SIZE; i++){
        cIndex[i].node = NULL;
    }
    // initialize the head node.
    head = new CINode();
    head->t = 0;
    head->rs = 0;
    head->pre = NULL;
    head->next = NULL;
    cIndex[0].node = head;
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
        if(pre->next != NULL){
            pre->next->pre = result;
        }
        result->next = pre->next;
        result->pre = pre;
        pre->next = result;
        // update the index
        unsigned int resultIndexLoc = getIndexLoc(t);
        unsigned int preIndexLoc = getIndexLoc(pre->t);
        if(preIndexLoc != resultIndexLoc){
            cIndex[resultIndexLoc].node = result;
        }
    }
    return result;
}

bool CILink::accept(Request r, CINode*& pre2st, CINode*& pre2et) {
    unsigned int st, et;
    CINode* temp;
    unsigned int indexLoc;

    st = iCurrentTime + r.ts;
    et = st + r.td;
    // TODO: need to be confirmed twice. > or >=
    if(et > iCurrentTime + CI_MAX_RESERVE_TIME){
        return false;
    }
    // find the first usable index.
    indexLoc = getIndexLoc(st);
    while(cIndex[indexLoc].node == NULL){
        // move index location forward.
//		unsigned int temp = et;
//		for (int var = 0; var < 100; ++var) {
//			temp *=2;
//			et = temp/2;
//		}

        if(indexLoc > 0){
            indexLoc = indexLoc - 1;
        }else{
            indexLoc = CI_INDEX_ARRAY_SIZE - 1;
        }
    }
    // find the last node which is not after the start time.
    temp = cIndex[indexLoc].node;
    if(temp->t > st){
        pre2st = temp->pre;
    }else{
        while(temp->next != NULL){
            if(temp->next->t > st){
                break;
            }
            temp = temp->next;
        }
        pre2st = temp;
    }
    // decide to accept or not.
    temp = pre2st;
    while(temp->next != NULL){
        // if the resource if not enough, return NULL.
        if(temp->rs + r.bw > iMaxResource){
            return false;
        }
        // if the next node is after the end time, break out.
        if(temp->next->t >= et){
            break;
        }else{
            temp = temp->next;
        }
    }
    // set the pre2et to match the requirement of insertNode function
    if(temp->next != NULL){
        if(temp->next->t == et){
            pre2et = temp->next;
        }else{
            pre2et = temp;
        }
    }else{
        pre2et = temp;
    }

    return true;
}

bool CILink::SetTime(unsigned int t) {
    if(t <= iCurrentTime){
        return false;
    }
    iCurrentTime = t;
    unsigned int iCurrentIndexNum = iCurrentTime / CI_INDEX_INTERVAL;
    for(; iStartIndex + 1 < iCurrentIndexNum; iStartIndex++){
        clearIndex(iStartIndex);
    }
    return true;
}

// clear the index that unused. n means the index number which will be cleared.
bool CILink::clearIndex(unsigned int n) {
    // 0th. get the index location
    unsigned int loc = n % CI_INDEX_ARRAY_SIZE;
    unsigned int next = (n + 1) % CI_INDEX_ARRAY_SIZE;
    // 1st. reset the index
    this->cIndex[loc].node = NULL;
    // 2nd. free the nodes during index n.
    // get the end time
    unsigned int et = (n + 1) * CI_INDEX_INTERVAL;
    // find the last node before end time
    CINode* startNode = head;
    while(startNode->next != NULL){
        if(startNode->next->t >= et){
            startNode->pre = NULL;
            break;
        }
        startNode = startNode->next;
        delete startNode->pre;
    }
    // at this step, the startNode is the last node before end time.
    // Then if the head node is not the last node, replace the head with startNode and delete the original head node.
    head = startNode;

    /*
     // a way to not change the head node.
     if (head->next != NULL) {
     if (head->next->t<et) {
     startNode = head->next;
     while (startNode->next != NULL) {
     if (startNode->next->t >= et) {
     startNode->pre = NULL;
     break;
     }
     startNode = startNode->next;
     delete startNode->pre;
     }
     // at this step, the startNode is the last node before end time.
     // Then if the head node is not the last node, replace the head with startNode and delete the original head node.
     head->t = startNode->t;
     head->rs = startNode->rs;
     head->next = startNode->next;
     if (startNode->next != NULL) {
     startNode->next->pre = head;
     }
     delete startNode;
     }
     }
     */
    // 3rd. link head to the next index
    this->cIndex[next].node = head;
    // return true. there is not any other result.
    return true;
}

unsigned int CILink::getIndexLoc(unsigned int t) {
    return (t / CI_INDEX_INTERVAL) % CI_INDEX_ARRAY_SIZE;
}
