#include "Capacaitor.h"
Capacaitor::Capacaitor(double r,double f, string b, int n1, int n2)
	:Component(b, n1, n2)
{
	w = f;
	admittance.real(0);
	impedance.real(0);
	capacitance = r;
}
void Capacaitor::set_Impedance(double r)
{
	impedance.real(0);
	impedance.imag(r);
	admittance.real(0);
	admittance.imag(-1/r);
}
complex <double> Capacaitor::get_Impedance()
{
	impedance.imag(-1 / (capacitance * w));
	return impedance;
}
complex <double> Capacaitor::get_Admittance()
{
	admittance.imag(capacitance * w);
	return admittance;
}