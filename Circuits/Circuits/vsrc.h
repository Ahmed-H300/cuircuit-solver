#pragma once
#include<complex>
#include"Node.h"
#include "Component.h"
#include "Indepentsourcr.h"
class Vsrc :public Indepentsourcr
{
private:
public:
	Vsrc(string, int node1, int node2,double pahse ,double magnitude);

	
	~Vsrc();
};

