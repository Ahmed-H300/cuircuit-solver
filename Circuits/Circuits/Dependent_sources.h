#pragma once
#include"Component.h"

class Dependent_sources: public Component
{
private:
	int node_d1;
	int node_d2;
	double cofficient;
public:
	Dependent_sources(int x1, int x2, string n, int d1, int d2, double coff);
	void set_Noded1(int x);
	void set_Noded2(int x);
	void set_cofficient(double x);
	int get_Noded1();
	int get_Noded2();
	double get_cofficient();
};