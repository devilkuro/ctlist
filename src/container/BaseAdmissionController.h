/*
 * BaseAdmissionController.h
 *
 *  Created on: 2015-2-12
 *      Author: Administrator
 */

#ifndef BASEADMISSIONCONTROLLER_H_
#define BASEADMISSIONCONTROLLER_H_
#include "common.h"
class BaseAdmissionController {
public:
    BaseAdmissionController();
    virtual ~BaseAdmissionController();

    virtual void setTime(unsigned int time) = 0;
    virtual bool accept(Request request) = 0;
    virtual bool forceInsert(Request request) = 0;
    virtual bool insert(Request request) = 0;
    virtual void setResourceCap(unsigned int maxResource) = 0;
    virtual bool Output(); // display the link list.
};

#endif /* BASEADMISSIONCONTROLLER_H_ */
