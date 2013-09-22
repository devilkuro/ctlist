/*
 * ctlink.cc
 *
 *  Created on: 2013-9-18
 *      Author: Fanjing
 */

#ifndef _CTLINK_H_
#define _CTLINK_H_

#include "common.h"

struct CTNode{
	unsigned int rs; // rs means the remainder resource.
	CTNode* pre; //point to pre node.
	CTNode* next; //point to next node.
};

struct CTTack{
	bool indexed; //be true, if the node after this tack is indexed.
				  //To improve the performance, it can be replaced by several bits in one int type.
	CTNode* node; //point to the first node after this tack.
	CTNode** index; //this will point to the index of this tack if there is one, else this will be null.
};

class CTLink{
public:
	CTLink();
	~CTLink();
public:
	bool Insert(Request r); //return true if success.
private:
	CTTack* tack;

};


#endif /* CTLINK_H_ */
