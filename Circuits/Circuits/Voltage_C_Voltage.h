#pragma once
#include "Dependent_sources.h"
class Voltage_C_Voltage :public Dependent_sources
{
public:
	Voltage_C_Voltage(int x1, int x2, string n, int d1, int d2, double coff);
};

