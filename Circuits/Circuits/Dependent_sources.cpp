#include "Dependent_sources.h"
Dependent_sources::Dependent_sources(int x1, int x2, string n, int d1, int d2, double coff)
	:Component (n,x1,x2)
{
	node_d1 = d1;
	node_d2 = d2;
	cofficient = coff;
}
void Dependent_sources::set_Noded1(int x)
{
	node_d1 = x;
}
void Dependent_sources::set_Noded2(int x)
{
	node_d2 = x;
}
void Dependent_sources::set_cofficient(double x)
{
	cofficient = x;
}
int Dependent_sources::get_Noded1()
{
	return node_d1;
}
int Dependent_sources::get_Noded2()
{
	return node_d2;
}
double Dependent_sources::get_cofficient()
{
	return cofficient;
}
