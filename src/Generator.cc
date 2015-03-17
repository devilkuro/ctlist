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
    td_down = 100;
    td_up = 100;
    interval = 100;
    hp.setSeed(0);
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
    if(file == NULL)
        return false;
    unsigned int size = sizeof(double) + sizeof(Request);
    buff = new char[size * t];
    for(unsigned i = 0; i < t; i++){
        interval = this->getNext(&rq);
#ifdef GN_OUT_DEBUG
        if (i % 100000 == 0 && DEBUG){
            cout << "write:: Request(" << rq.bw << " , " << rq.ts << " , "
            << rq.td << " ):" << interval << endl;
        }
#endif
        memcpy(buff + i * size, &interval, sizeof(double));
        memcpy(buff + i * size + sizeof(double), &rq, sizeof(Request));
    }
    file.write(buff, size * t);
#ifdef GN_OUT_DEBUG
    cout << "total write: " << size * t << endl;
#endif
    delete (buff);
    file.close();
    return true;
}

unsigned int Generator::getNext(Request* rq) {
    rq->bw = hp.U_Randint(bw_down, bw_up);
    rq->ts = hp.U_Randint(ts_down, ts_up);
    // rq->td = (unsigned int) hp.E_Rand(1.0 / td_ave) % td_limit;
    rq->td = (unsigned int) hp.F_Rand(td_down,td_up);
    // fixed at 201503161648.
    if(rq->td<1){
        rq->td = 1;
    }
    return (unsigned int) hp.P_Rand(interval);
}

void Generator::setGenerator(unsigned int bw_down, unsigned int bw_up,
        unsigned int ts_down, unsigned int ts_up, unsigned int td_ave,
        unsigned int td_limit, unsigned int interval,unsigned int seed) {
    this->bw_down = bw_down;
    this->bw_up = bw_up;
    this->ts_down = ts_down;
    this->ts_up = ts_up;
    this->td_down = td_ave;
    this->td_up = td_limit;
    this->interval = interval;
    hp.setSeed(seed);
}
