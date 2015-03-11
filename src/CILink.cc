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
    cout<<"~CILink"<<endl;
    // free the link list
    while(head != NULL){
        CINode* temp = head;
        head = head->next;
        delete temp;
    }
    // free the index array
    delete[] (cIndex);
}

bool CILink::insert(Request r) {
    // accept judgment.
    if(r.td == 0){
        return true;
    }

    if(!accept(r)){
        return false;
    }else{
        forceInsert(r);
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

bool CILink::accept(Request r) {
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
        if(indexLoc > 0){
            indexLoc = indexLoc - 1;    // backward traversal
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

void CILink::setTime(unsigned int t) {
    if(t <= iCurrentTime){
        return;
    }
    iCurrentTime = t;
    unsigned int iCurrentIndexNum = iCurrentTime / CI_INDEX_INTERVAL;
    for(; iStartIndex + 1 < iCurrentIndexNum; iStartIndex++){
        cleanIndex(iStartIndex);
    }
    return;
}

// clear the index that unused. n means the index number which will be cleared.
bool CILink::cleanIndex(unsigned int n) {
    // 0th. get the index location
    unsigned int loc = n % CI_INDEX_ARRAY_SIZE;
    // update at 201503021021, remove mod operation to increase the efficiency
    unsigned int next = loc == CI_INDEX_ARRAY_SIZE - 1 ? 0 : loc + 1;
    // 1st. reset the index
    this->cIndex[loc].node = NULL;
    
    // 2nd. delete the nodes during index n.
    // get the end time
    unsigned int et = (n + 1) * CI_INDEX_INTERVAL;
    // delete the nodes one more before end time
    CINode* temp = head;
    while(temp->next != NULL && temp->next->t < et){
        temp = temp->next;
        delete temp->pre;
    }
    temp->pre = NULL;
    // at this step, the temp is the last node before end time.
    // then replace the head with temp.
    head = temp;
    
    // 3rd. link head to the next index
    this->cIndex[next].node = head;
    // return true. there is not any other result.
    return true;
}

bool CILink::forceInsert(Request r) {
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
    return true;
}

unsigned int CILink::getIndexLoc(unsigned int t) {
    return (t / CI_INDEX_INTERVAL) % CI_INDEX_ARRAY_SIZE;
}

void CILink::setResourceCap(unsigned int maxResource) {
	iMaxResource = maxResource;
}
