#pragma once
#include "Dependent_sources.h"
class Current_C_Current :public Dependent_sources
{
private:
	complex <double> Impedance ;
	string Name2;
public:
	Current_C_Current(int x1, int x2, string n, int d1, int d2, string name,double coff);
	void set_Impedance(complex <double> z)
	{
		Impedance = z;
	}
	void set_name(string l)
	{
		Name2 = l;
	}
	string get_name()
	{
		return Name2;
	}
	complex <double> get_Impedance()
	{
		return Impedance;
	}
	complex <double> get_Admittance()
	{
		return 1.0 / Impedance;
	}
};

