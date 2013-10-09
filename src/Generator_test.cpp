/*
 * Generator_test.cpp
 *
 *  Created on: 2013-9-20
 *      Author: Fanjing
 */

#include "Generator.h"
#define GN_DEBUG
#define DEBUG false
#ifdef GN_DEBUG
int main() {
#ifdef DEBUG
	unsigned int size; //size of each block.
	char* buff;
	unsigned long length;//total size
	Request* rq;
	double* interval;
	char* z;
	clock_t t_start, t_write, t_read, t_total;

	t_start = clock();
#endif
	Generator* gn = new Generator();
	string fileName("output.dat");
	gn->output(fileName.c_str(), MAX_REQUEST_NUM);
#ifdef DEBUG
	t_write = clock() - t_start;

	ifstream file("output.dat");
	file.seekg(0, file.end);
	length = file.tellg();
	file.seekg(0, file.beg);

	size = (sizeof(double) + sizeof(Request) + sizeof(char));
	buff = new char[length];
	file.read(buff, length);

	t_read = clock() - t_start - t_write;

	for (unsigned int i = 0; i < length / size; i++) {
		interval = (double*) (buff + i * size);
		rq = (Request*) (buff + i * size + sizeof(double));
		z = (char*) (buff + i * size + sizeof(double) + sizeof(Request));
		if (i % 100000 == 0 && DEBUG) {
			cout << "read:: Request(" << rq->bw << " , " << rq->ts << " , "
			<< rq->td << " ):" << *interval << "+" << *z + 0 << endl;
		}
	}
	file.close();

	delete(buff);
	t_total = clock() - t_start;
	cout << "function started at " << t_start << endl;
	cout << "generate and write finished in " << t_write
	<< " ms and read finished in " << t_read << " ms." << endl;
	cout << "function finished in " << t_total << " ms." << endl;
#endif

	delete(gn);
	return 0;
}
#endif
