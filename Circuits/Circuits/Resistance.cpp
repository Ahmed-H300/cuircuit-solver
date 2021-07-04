#include "Resistance.h"

Resistance::Resistance(double r, string b, int n1 , int n2 )
	:Component(b,n1,n2)
{
	z.imag(0);
	z.real(r);
}
void Resistance::set_Impedance(double r)
{

	z.real(r);
	z.imag(0);
}
complex <double> Resistance::get_Admittance()
{
	return( 1.0/z);
}
complex <double> Resistance::get_Impedance()
{
	return z;
}
