/*
 * BaseAdmissionController.cc
 *
 *  Created on: 2015年2月12日
 *      Author: Administrator
 */

#include "BaseAdmissionController.h"

BaseAdmissionController::BaseAdmissionController() {
	// TODO Auto-generated constructor stub

}

BaseAdmissionController::~BaseAdmissionController() {
	// TODO Auto-generated destructor stub
}

void BaseAdmissionController::setTime(unsigned int time) {
}

bool BaseAdmissionController::accept(Request request) {
	return false;
}

bool BaseAdmissionController::forceInsert(Request request) {
	return false;
}

bool BaseAdmissionController::insert(Request request) {
	return false;
}
