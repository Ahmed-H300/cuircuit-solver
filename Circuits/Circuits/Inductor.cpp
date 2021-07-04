#include "Inductor.h"

Inductor::Inductor(double r, double f, string b, int n1, int n2)
	:Component(b, n1, n2)
{
	w = f;
	admittance.real(0);
	impedance.real(0);
	inductance = r;
}
void Inductor::set_Impedance(double r)
{
	impedance.real(0);
	impedance.imag(r);
	admittance.real(0);
	admittance.imag(-1 / r);
}
complex <double> Inductor::get_Admittance()
{
	admittance.imag(-1 / (inductance * w));
	return admittance;
}
complex <double> Inductor::get_Impedance()
{
	impedance.imag(inductance * w);
	return impedance;
}