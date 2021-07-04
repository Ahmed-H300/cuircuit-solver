#pragma once
#include"Component.h"
class Inductor :public Component
{
private:
	double inductance;
	double w;
	complex<double> impedance;
	complex<double> admittance;

public:
	Inductor(double, double, string, int,int);
	void set_Impedance(double);
	complex <double> get_Impedance();
	complex <double> get_Admittance();
	~Inductor()
	{

	}
};


