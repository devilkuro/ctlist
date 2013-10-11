/*
 * Generator.cpp
 *
 *  Created on: 2013-9-20
 *      Author: Fanjing
 */

#include "Generator.h"

Generator::Generator() {
	// initialize all members here.
	bw_down = 100;
	bw_up = 1000;
	ts_down = 20;
	ts_up = 50;
	td_ave = 100;
	td_limit = 100;
	interval = 100;
}

Generator::~Generator() {
	// there is nothing to do here at now.
}

bool Generator::output(const char* fileName, unsigned int t) {
	//finished at 2013-9-22 12:58:06
	Request rq;
	char* buff;
	double interval;
	ofstream file(fileName, ios_base::binary);
	if (file == NULL)
		return false;
	unsigned int size = sizeof(double) + sizeof(Request);
	buff = new char[size * t];
	for (unsigned i = 0; i < t; i++) {
		interval = this->getNext(&rq);
#ifdef DEBUG
		if (i % 100000 == 0 && DEBUG) {
			cout << "write:: Request(" << rq.bw << " , " << rq.ts << " , "
					<< rq.td << " ):" << interval << endl;
		}
#endif
		memcpy(buff + i * size, &interval, sizeof(double));
		memcpy(buff + i * size + sizeof(double), &rq, sizeof(Request));
	}
	file.write(buff, size * t);
#ifdef DEBUG
	cout << "total write: " << size * t << endl;
#endif
	file.close();
	return true;
}

unsigned int Generator::getNext(Request* rq) {
	Helper hp;
	rq->bw = hp.U_Randint(bw_down, bw_up);
	rq->ts = hp.U_Randint(ts_down, ts_up);
	rq->td = (unsigned int) hp.E_Rand(1.0/td_ave) % td_limit;
	return (unsigned int )hp.P_Rand(interval);
}

