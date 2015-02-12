/*
 * BaseAdmissionController.h
 *
 *  Created on: 2015年2月12日
 *      Author: Administrator
 */

#ifndef BASEADMISSIONCONTROLLER_H_
#define BASEADMISSIONCONTROLLER_H_
#include "common.h"
class BaseAdmissionController {
public:
    BaseAdmissionController();
    virtual ~BaseAdmissionController();

    virtual void setTime(unsigned int time);
    virtual bool accept(Request request);
    virtual bool forceInsert(Request request);
    virtual bool insert(Request request);
};

#endif /* BASEADMISSIONCONTROLLER_H_ */
