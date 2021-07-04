#pragma once
#include"Component.h"

class Resistance :public Component
{
private:
	complex<double> z;
public:
	Resistance(double,string, int, int);
	void set_Impedance(double);
	complex <double> get_Impedance();
	complex <double> get_Admittance();
	~Resistance()
	{

	}
};

