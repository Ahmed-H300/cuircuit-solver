#pragma once
#include <iostream>
#include <complex>       
#include "Node.h"
#include "isrc.h"
#include"Current_C_Current.h"
using namespace std;

#include"vsrc.h"
class Branch {
private:
	int num = 0;
	Node* node1; Node* node2;
	complex <double>z;
	complex <double> current;
	Vsrc *volt;
	Isrc* csource;
	Current_C_Current* dependentcsource;

public:
	Branch(Node*, Node*);
	void setnode1(Node* N1);
	void setNode2(Node* N2);
	void setz(complex<double> x);
	void setcurrent(complex<double> c);
	void setvolt(Vsrc *c);
	void setcurentsource(Isrc*);
	void setDependentCsource(Current_C_Current*);
	Node* getnode1();
	Node* getnode2();
	complex <double>getz();
	Vsrc *getvolt();
	Isrc* getcsource();
	Current_C_Current* setDependentCsource();
	complex<double> getcurrent();
	////////////// sabry

	void Calculatecurrent();
	complex <double> get_Admittance()
	{
		if (z == 0.0 + 0.0i)
		{
			return z;
		}
		return 1.0 / z;
	}

	///////////////
	int getnum() {
		return num;
	}
};

