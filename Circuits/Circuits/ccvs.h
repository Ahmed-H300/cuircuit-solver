#pragma once
#include "Component.h"
class ccvs :	public Component
{
private:
	int n1;
	int n2;
	int n3;
	int n4;
	string name;
	string nimp;
	double coff;
	complex<double> x;
public:
	ccvs(string, int, int,int,int, string, double);
	void setnode1(int);
	void setnode2(int);
	void setnode3(int);
	void setnode4(int);
	double getcoff();
	complex<double> getcomplex(double mag, double phase);
	~ccvs();
};

