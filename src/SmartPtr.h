/*
 * SmartPtr.h
 *
 *  Created on: 2013-10-4
 *      Author: Fanjing
 */

#ifndef _SMARTPTR_H_
#define _SMARTPTR_H_

#include <stdexcept>
//#define TEST_SMARTPTR
/*
 *
 */
template<typename T>

class SmartPtr {
public:
	SmartPtr(T *p = 0) :
			ptr(p), pUse(new size_t(1)) {
	}
	SmartPtr(const SmartPtr& src) :
			ptr(src.ptr), pUse(src.pUse) {
		++*pUse;
	}
	SmartPtr& operator=(const SmartPtr& rhs) {
	// self-assigning is also right
		++*rhs.pUse;
		decrUse();
		ptr = rhs.ptr;
		pUse = rhs.pUse;
		return *this;
	}
	bool operator !() const { return !ptr; }
	bool operator ==(const SmartPtr& rhs) const { return rhs.ptr==ptr; }
	bool operator !=(const SmartPtr& rhs) const { return rhs.ptr!=ptr; }
	operator bool() const { return (bool)ptr!=0; }

	T *operator->() {
		if (ptr)
			return ptr;
		throw std::runtime_error("access through NULL pointer");
	}
	const T *operator->() const {
		if (ptr)
			return ptr;
		throw std::runtime_error("access through NULL pointer");
	}
	T &operator*() {
		if (ptr)
			return *ptr;
		throw std::runtime_error("dereference of NULL pointer");
	}
	const T &operator*() const {
		if (ptr)
			return *ptr;
		throw std::runtime_error("dereference of NULL pointer");
	}
	~SmartPtr() {
		decrUse();
	}
private:
	void decrUse() {
		if (--*pUse == 0) {
			delete ptr;
			delete pUse;
#ifdef TEST_SMARTPTR
			std::cout << "SmartPtr: Destructor" << std::endl; // for testing
#endif
		}
	}
	T *ptr;
	size_t*pUse;
};

#endif /* _SMARTPTR_H_ */
