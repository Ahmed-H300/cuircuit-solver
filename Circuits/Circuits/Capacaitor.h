#pragma once
#include"Component.h"
class Capacaitor :public Component
{
private:
	double capacitance;
	double w;
	complex<double> impedance;
	complex<double> admittance;

public:
	Capacaitor(double,double, string, int, int);
	void set_Impedance(double);
	complex <double> get_Impedance();
	complex <double> get_Admittance();
	~Capacaitor()
	{

	}
};


