#pragma once
#include<complex>
#include "Node.h"
#include "Component.h"
#include "Indepentsourcr.h"

class Isrc : public Indepentsourcr
{
private:


public:
	Isrc(string, int, int, double, double);


	~Isrc();
	
};

