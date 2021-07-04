#include "Indepentsourcr.h"
#include<cmath>
Indepentsourcr::Indepentsourcr(string c, int a, int b, double x, double y) :Component(c, a, b) {

	phase = x;
	mag = y;
}
double Indepentsourcr::getmag() {
	return mag;
}

double Indepentsourcr::getphase() {
	return phase;
}
complex<double> Indepentsourcr::getcomplex() {
	double angle = (phase / 180) * 3.14;
	x.real(mag * cos(angle));
	x.imag(mag * sin(angle));
	return x;
}
Indepentsourcr::~Indepentsourcr() {}
