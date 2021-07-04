#include "Branch.h"
#include <iostream>
#include <complex>       
#include "Node.h"
Branch::Branch(Node* N1, Node* N2)
{
	z = 0;
	node1 = N1;
	node2 = N2;
	csource = NULL;
	volt = NULL;
}
void Branch:: setnode1(Node* N1)
{
	node1 = N1;
}
void Branch::setNode2(Node* N2)
{
	node2 = N2;
}
void Branch::setz(complex<double> x)
{
	z += x;
}
void Branch::setcurrent(complex<double> c)
{
	current = c;
}
void Branch::setcurentsource(Isrc* i)
{
	csource = i;
}

void Branch::setDependentCsource(Current_C_Current*D_I)
{
	dependentcsource = D_I;
}
Node* Branch::getnode1()
{
	return node1;
}
Node* Branch::getnode2()
{
	return node2;
}
complex <double>Branch::getz()
{
	return z;
}
complex<double> Branch::getcurrent()
{
	return current;
}
void Branch::Calculatecurrent()
{
	if (z != 0.0 + 0.0i)
	{
		current = (node1->getvoltage() - node2->getvoltage()) / z;
	}
	else if(csource !=NULL)
	{
		current = csource->getcomplex();
	}
	else
	{
		current = 0;
	}
}
void Branch::setvolt(Vsrc *c)
{
	volt = c;
}
Vsrc *Branch::getvolt()
{
	return volt;
}
Isrc* Branch::getcsource()
{
	return csource;
}
Current_C_Current* Branch::setDependentCsource()
{
	return dependentcsource;
}