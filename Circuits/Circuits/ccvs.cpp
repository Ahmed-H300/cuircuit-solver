#include "ccvs.h"
ccvs::ccvs(string x, int a, int b, int c, int d, string r, double y) :Component(x, a, b) {
	name = x;
	n1 = a;
	n2 = b;
	n3 = c;
	n4 = d;
	nimp = r;
	coff = y;
}
void ccvs::setnode1(int a) {
	n1 = a;
}
void ccvs::setnode2(int a) {
	n2 = a;
}
void ccvs::setnode3(int a) {
	n3 = a;
}
void ccvs::setnode4(int a) {
	n4 = a;
}
double ccvs::getcoff() {
	return coff;
}

complex<double> ccvs::getcomplex(double mag,double phase) {
	x.real(mag * cos(phase));
	x.imag(mag * sin(phase));
	return x;
}
ccvs::~ccvs() {};