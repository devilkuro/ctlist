/*
 * Generator.cpp
 *
 *  Created on: 2013-9-20
 *      Author: Fanjing
 */

#include "Generator.h"

Generator::Generator() {
	// TODO initialize the parameters here.
}

Generator::~Generator() {
	// TODO Auto-generated destructor stub
}

bool Generator::output(const char* fileName, unsigned int t) {
	//finished at 2013-9-22 12:58:06
	Request rq;
	char* buff;
	double interval;
	ofstream file(fileName, ios_base::binary);
	if (file == NULL)
		return false;
	unsigned int size = (sizeof(double) + sizeof(Request) + sizeof(char));
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
		buff[i * size + sizeof(double) + sizeof(Request)] = 0;
	}
	file.write(buff, size * t);
#ifdef DEBUG
	cout << "total write: " << size * t << endl;
#endif
	file.close();
	return true;
}

double Generator::getNext(Request* rq) {
	Helper hp;
	rq->bw = hp.U_Randint(100, 1000);
	rq->ts = hp.U_Randint(20, 50);
	rq->td = (int) hp.E_Rand(0.01) % 100;
	return hp.P_Rand(100);
}

