#pragma once
#include "Component.h"
class Indepentsourcr : public Component
{
private:

	double phase;
	double mag;
	complex<double> x;

public:
	Indepentsourcr(string, int, int, double, double);

	void zerosetters() {
		phase = mag = 0;
		x.real(0);
		x.imag(0);

	}
	double getmag();
	double getphase();
	complex<double> getcomplex();
	void setcomplex(complex <double>y)
	{
		x = y;
		double p = x.real() * x.real() + x.imag() * x.imag();
		mag = sqrt(p);
		phase = atan(x.imag() / x.real()) * 180 / 3.14;
	}
	~Indepentsourcr();
};

