/*
 * CArrayList.h
 *
 *  Created on: 2015-2-12
 *      Author: Fanjing-LAB
 */

#ifndef _CARRAYLIST_H_
#define _CARRAYLIST_H_

#include "common.h"
#include "BaseAdmissionController.h"

class CArrayList : public BaseAdmissionController {
public:
    CArrayList();
    CArrayList(unsigned int size, unsigned int scale);
    virtual ~CArrayList();
public:
    bool insert(Request request);
    void setTime(unsigned int time);
    bool accept(Request request);
    bool forceInsert(Request request);
    virtual void setResourceCap(unsigned int maxResource);
protected:
    unsigned int getIndex(unsigned int t);
public:
    unsigned int max_resource;
private:
    unsigned int m_scale;
    unsigned int m_time;
    unsigned int *m_resource;
    unsigned int m_array_size;
};

#endif /* _CARRAYLIST_H_ */
