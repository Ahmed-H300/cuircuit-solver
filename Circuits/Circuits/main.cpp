#include<iostream>
#include <cmath>
#include<string>
#include<fstream>
#include<Eigen/Dense>
#include<Eigen/Sparse>
#include<complex>
#include"vsrc.h"
#include"Capacaitor.h"
#include"Resistance.h"
#include"Inductor.h"
#include"isrc.h"
#include"Branch.h"
#include "Node.h"
#include"Dependent_sources.h"
#include"Current_C_Current.h"
#include"Voltage_C_Voltage.h"
using namespace std;
using namespace Eigen;
using namespace std::complex_literals;
using namespace std;
#include"Component.h"
double W;
int searcher(string x);
void Save( Node**, Branch**, int, int );
#define n_max 10
Component* complist[n_max] = { NULL };
int comcount = 0;
/////////////nodes&branches/////////////
int nodecount = 0;
int nonsimple_nodecount = 0;
int simple_nodecount = 0;
int branchcount = 0;
int nodes_rannk[10] = { -1 ,-1,-1,-1,-1,-1,-1,-1,-1,-1 };
Node** Nodelist;
Node** nonsimple_Nodelist;
Node** simple_Nodelist;
Branch** fullBranchlist;
Branch** Branchlist;
/////////////nodes&branches/////////////
Isrc* simplification(Vsrc* a, complex<double> b, Node* N1, Node* N2)
{
	complex<double> x = (a->getcomplex()) / b;
	double p = x.real() * x.real() + x.imag() * x.imag();
	double mag = sqrt(p);
	double phase = atan(x.imag() / x.real()) * 180 / 3.14;
	Isrc* c = NULL;
	if(N2->getrank() == a->get_node2() || N1->getrank() == a->get_node1())
	{
		c = new Isrc(a->get_name(), N1->getrank(), N2->getrank(), phase, mag);
	}
	else
	{
		c = new Isrc(a->get_name(), N2->getrank(), N1->getrank(), phase, mag);

	}
	Branchlist[branchcount] = new Branch(N1, N2);
	Branchlist[branchcount]->setcurentsource(c);
	branchcount++;
	return c;
}
MatrixXcd CalculateVoltNode(MatrixXcd G, MatrixXcd I);

int main()
{
	ifstream inputfile;
	inputfile.open("testcase2.txt");
	if (!(inputfile.is_open()))
	{
		cout << "file is not opened , try again" << endl;
		return 0;
	}
	string name;

	while (!(inputfile.eof()))
	{
		int n1 = -1, n2 = -1;
		inputfile >> name;
		if (name == "w"|| name =="W")
		{
			double w;
			inputfile >> w;
			W = w;
		}
		else if (name == "vsrc" || name == "Vsrc" || name == "VSrc")
		{
			string Name;
			double phase, magintude;
			inputfile >> Name >> n1 >> n2 >> magintude >> phase;
			Vsrc* v1 = new Vsrc(Name, n1, n2, phase, magintude);
			complist[comcount++] = v1;
		}
		else if (name == "isrc" || name == "Isrc" || name == "ISrc")
		{
			string Name;
			double phase, magintude;
			inputfile >> Name >> n1 >> n2 >> magintude >> phase;
			Isrc* v1 = new Isrc(Name, n1, n2, phase, magintude);
			complist[comcount++] = v1;
		}
		else if (name == "vcvs" || name == "Vcvs" || name == "VCvs")
		{
			string Name;
			int n3, n4;
			string Name2;
			double val;
			inputfile >> Name >> n1 >> n2 >> n3 >> n4 >> val;
			Voltage_C_Voltage* v1 = new Voltage_C_Voltage(n1, n2, name, n3, n4, val);
			complist[comcount++] = v1;
		}
		else if (name == "cccs" || name == "Cccs" || name == "CCcs")
		{
			string Name;
			int n3, n4;
			string Name2;
			double val;
			inputfile >> Name >> n1 >> n2 >> n3 >> n4 >> Name2 >> val;
			Current_C_Current* cccs = new Current_C_Current(n1,n2,Name,n3,n4,Name2,val);
			complist[comcount++] = cccs;
		}
		else if (name == "res")
		{
			string Name;
			double val;
			inputfile >> Name >> n1 >> n2 >> val;
			Resistance* v1 = new Resistance(val, Name, n1, n2);
			complist[comcount++] = v1;
		}
		else if (name == "cap")
		{
			string Name;
			double val;
			inputfile >> Name >> n1 >> n2 >> val;
			Capacaitor* v1 = new Capacaitor(val, W, Name, n1, n2);
			complist[comcount++] = v1;
		}
		else if (name == "ind")
		{
			string Name;
			double val;
			inputfile >> Name >> n1 >> n2 >> val;
			Inductor* v1 = new Inductor(val, W, Name, n1, n2);
			complist[comcount++] = v1;
		}
		bool findN1 = false;
		bool findN2 = false;
		for (int i = 0; i < 10; i++)
		{
			if (n1 == nodes_rannk[i])
			{
				findN1 = true;
			}
			if (n2 == nodes_rannk[i])
			{
				findN2 = true;
			}
		}
		if (findN1 == false)
		{
			nodes_rannk[nodecount++] = n1;
		}
		if (findN2 == false)
		{
			nodes_rannk[nodecount++] = n2;
		}

	}
	inputfile.close();

	Nodelist = new Node * [nodecount];
	for (int i = 0; i < nodecount; i++)
	{
		Nodelist[i] = new Node(i);
	}

	for (int i = 0; i < comcount; i++)
	{
		int N1 = complist[i]->get_node1();
		int N2 = complist[i]->get_node2();
		int indexnode1, indexnode2;
		for (int j = 0; j < nodecount; j++)
		{
			if (Nodelist[j]->getrank() == N1)
			{
				indexnode1 = j;
			}
			else if (Nodelist[j]->getrank() == N2)
			{
				indexnode2 = j;
			}
		}
		Nodelist[indexnode1]->increment_connections(Nodelist[indexnode2]);
		Nodelist[indexnode2]->increment_connections(Nodelist[indexnode1]);
	}
	for (int j = 0; j < nodecount; j++)
	{
		if (!(Nodelist[j]->ifsimple()))
		{
			nonsimple_nodecount++;
		}
	}
	simple_nodecount = nodecount - nonsimple_nodecount;
	nonsimple_Nodelist = new Node * [nonsimple_nodecount];
	simple_Nodelist = new Node * [simple_nodecount];

	{
		int nonsimple = 0; int simple = 0;
		for (int i = 0; i < nodecount; i++)
		{
			if (Nodelist[i]->ifsimple())
			{
				simple_Nodelist[simple++] = Nodelist[i];
			}
			else
			{
				nonsimple_Nodelist[nonsimple++] = Nodelist[i];
			}
		}
	}

	////////////////////////////// creat branches /////////////////////////////////////
	branchcount = comcount - simple_nodecount;
	Branchlist = new Branch * [comcount];
	for (int i = 0; i < comcount; i++)
	{
		Branchlist[i] = NULL;
	}
	{
		int counter = 0;
		for (int i = 0; i < comcount; i++)
		{
			int N1 = complist[i]->get_node1();
			int N2 = complist[i]->get_node2();
			bool find = false;
			for (int j = 0; j < simple_nodecount; j++)
			{
				if (N1 == simple_Nodelist[j]->getrank() || N2 == simple_Nodelist[j]->getrank())
				{
					find = true;
				}
			}
			if (find == false)
			{
				Vsrc* v1 = dynamic_cast <Vsrc*>(complist[i]);
				Isrc* i1 = dynamic_cast <Isrc*>(complist[i]);
				Current_C_Current* D_I = dynamic_cast<Current_C_Current*>(complist[i]);
				Resistance* r1 = dynamic_cast <Resistance*>(complist[i]);
				Capacaitor* c1 = dynamic_cast <Capacaitor*>(complist[i]);
				Inductor* in1 = dynamic_cast <Inductor*>(complist[i]);

				Branchlist[counter] = new Branch(Nodelist[N1], Nodelist[N2]);
				if (v1 != NULL)
				{
					Branchlist[counter]->setvolt(v1);
				}
				else if (i1 != NULL)
				{
					Branchlist[counter]->setcurentsource(i1);
				}
				else if (r1 != NULL)
				{
					Branchlist[counter]->setz(r1->get_Impedance());
				}
				else if (c1 != NULL)
				{
					Branchlist[counter]->setz(c1->get_Impedance());
				}
				else if (in1 != NULL)
				{
					Branchlist[counter]->setz(in1->get_Impedance());
				}
				else if(D_I != NULL)
				{
					Branchlist[counter]->setDependentCsource(D_I);
				}
				counter++;
			}
		}
		////////////////// circuts that has only one loop
		if (nonsimple_nodecount == 0)
		{
			Branchlist[counter] = new Branch(Nodelist[0], Nodelist[0]);
			for (int j = 0; j < comcount; j++)
			{
				Vsrc* v1 = dynamic_cast <Vsrc*>(complist[j]);
				Isrc* i1 = dynamic_cast <Isrc*>(complist[j]);

				Resistance* r1 = dynamic_cast <Resistance*>(complist[j]);
				Capacaitor* c1 = dynamic_cast <Capacaitor*>(complist[j]);
				Inductor* in1 = dynamic_cast <Inductor*>(complist[j]);
				if (v1 != NULL)
				{
					if (Branchlist[counter]->getvolt() != NULL)
					{
						int N1 = Branchlist[counter]->getvolt()->get_node1();
						int N2 = Branchlist[counter]->getvolt()->get_node2();
						int N3 = v1->get_node1();
						Vsrc* sum_volt = new Vsrc("loop", N1, N2, Branchlist[counter]->getvolt()->getphase(), Branchlist[counter]->getvolt()->getmag());
						complex <double> curently_volt = sum_volt->getcomplex();

						int N4 = v1->get_node2();
						if (N1 == N4 || N2 == N3)
						{
							curently_volt += v1->getcomplex();
						}
						else if (N1 == N3 || N2 == N4)
						{
							curently_volt -= v1->getcomplex();
						}
						else
						{
							if (N1 > N2) {
								for (int i = 1; i < nodecount; i++)
								{
									if ((N1 + i) == N4)
									{
										curently_volt += v1->getcomplex();
										break;
									}
									else if ((N1 + i) == N3)
									{
										curently_volt -= v1->getcomplex();
										break;
									}
									else if ((N2 - i) == N4)
									{
										curently_volt -= v1->getcomplex();
										break;
									}
									else if ((N2 - i) == N3)
									{
										curently_volt += v1->getcomplex();
										break;
									}
								}
							}
							else
							{
								for (int i = 1; i < nodecount; i++)
								{
									if ((N1 - i) == N4)
									{
										curently_volt += v1->getcomplex();
										break;
									}
									else if ((N1 - i) == N3)
									{
										curently_volt -= v1->getcomplex();
										break;
									}
									else if ((N2 + i) == N4)
									{
										curently_volt -= v1->getcomplex();
										break;
									}
									else if ((N2 + i) == N3)
									{
										curently_volt += v1->getcomplex();
										break;
									}
								}
							}
						}
						if (curently_volt == 0.0)
						{
							sum_volt->zerosetters();
						}
						else
						{
							sum_volt->setcomplex(curently_volt);
						}
						Branchlist[counter]->setvolt(sum_volt);
					}
					else
					{
						Branchlist[counter]->setvolt(v1);
					}
				}
				else if (i1 != NULL)
				{
					Branchlist[counter]->setcurentsource(i1);
				}
				else if (r1 != NULL)
				{
					Branchlist[counter]->setz(r1->get_Impedance());
				}
				else if (c1 != NULL)
				{
					Branchlist[counter]->setz(c1->get_Impedance());
				}
				else if (in1 != NULL)
				{
					Branchlist[counter]->setz(in1->get_Impedance());
				}
			}
			//Isrc* i22 = Branchlist[counter]->getcsource();

			////////// set current in the loop  ////////////
			if (Branchlist[counter]->getcsource() != NULL)
			{
				Branchlist[counter]->setcurrent(Branchlist[counter]->getcsource()->getcomplex());
			}
			else
			{
				if (Branchlist[counter]->getvolt()->getmag() == 0.0)
				{
					//Branchlist[counter]->getvolt()->setcomplex((0, 0));
					Branchlist[counter]->setcurrent((0, 0));
				}
				else
				{
					Branchlist[counter]->setcurrent((Branchlist[counter]->getvolt()->getcomplex()) * (Branchlist[counter]->get_Admittance()));
				}
			}
			/*cout << r1 << endl;
			cout <<v1<<endl;
			cout << Branchlist[counter]->getcurrent() << endl;*/

			////////// set voltage for every node ////////////
			Nodelist[0]->setvoltage((0, 0));
			if (Branchlist[counter]->getcsource() != NULL)
			{
				Isrc* ic = Branchlist[counter]->getcsource();
				int Nc1 = ic->get_node1();
				int Nc2 = ic->get_node2();
				if (Nc1 > Nc2)
				{
					for (int i = 1; i <= Nc2; i++)
					{
						complex<double> v = Nodelist[i - 1]->getvoltage();
						Component* c_find = NULL;
						int pos_node = -1;
						int neg_node = -1;
						for (int j = 0; j < comcount; j++)
						{
							int N1 = complist[j]->get_node1();
							int N2 = complist[j]->get_node2();
							if ((N1 == Nodelist[i]->getrank() || N2 == Nodelist[i]->getrank()) && (N1 == Nodelist[i - 1]->getrank() || N2 == Nodelist[i - 1]->getrank()))
							{
								c_find = complist[j];
								break;
							}
						}
						if (c_find != NULL)
						{
							pos_node = c_find->get_node1();
							neg_node = c_find->get_node2();
							Vsrc* v1 = dynamic_cast <Vsrc*>(c_find);
							Isrc* i1 = dynamic_cast <Isrc*>(c_find);

							Resistance* r1 = dynamic_cast <Resistance*>(c_find);
							Capacaitor* c1 = dynamic_cast <Capacaitor*>(c_find);
							Inductor* in1 = dynamic_cast <Inductor*>(c_find);
							if (v1 != NULL)
							{
								if (pos_node == Nodelist[i]->getrank())
								{
									v += v1->getcomplex();
								}
								else
								{
									v -= v1->getcomplex();
								}
							}
							else if (r1 != NULL)
							{
								if (pos_node == Nodelist[i]->getrank())
								{
									v += r1->get_Impedance() * (Branchlist[counter]->getcurrent());
								}
								else
								{
									v -= r1->get_Impedance() * (Branchlist[counter]->getcurrent());
								}
							}
							else if (c1 != NULL)
							{
								if (pos_node == Nodelist[i]->getrank())
								{
									v += c1->get_Impedance() * (Branchlist[counter]->getcurrent());
								}
								else
								{
									v -= c1->get_Impedance() * (Branchlist[counter]->getcurrent());
								}
							}
							else if (in1 != NULL)
							{
								if (pos_node == Nodelist[i]->getrank())
								{
									v += in1->get_Impedance() * (Branchlist[counter]->getcurrent());
								}
								else
								{
									v -= in1->get_Impedance() * (Branchlist[counter]->getcurrent());
								}
							}
						}
						Nodelist[i]->setvoltage(v);
					}
					for (int i = nodecount - 1; i >= Nc1; i--)
					{
						complex<double> v = (0, 0);
						Component* c_find = NULL;
						int pos_node = -1;
						int neg_node = -1;
						for (int j = 0; j < comcount; j++)
						{
							int N1 = complist[j]->get_node1();
							int N2 = complist[j]->get_node2();
							if ((N1 == Nodelist[i]->getrank() || N2 == Nodelist[i]->getrank()) && (N1 != Nodelist[i - 1]->getrank() && N2 != Nodelist[i - 1]->getrank()))
							{
								c_find = complist[j];
								if (N1 != Nodelist[i]->getrank())
								{
									v = Nodelist[N1]->getvoltage();
								}
								else if (N2 != Nodelist[i]->getrank())
								{
									v = Nodelist[N2]->getvoltage();
								}
								break;
							}
						}
						if (c_find != NULL)
						{
							pos_node = c_find->get_node1();
							neg_node = c_find->get_node2();
							Vsrc* v1 = dynamic_cast <Vsrc*>(c_find);
							Resistance* r1 = dynamic_cast <Resistance*>(c_find);
							Capacaitor* c1 = dynamic_cast <Capacaitor*>(c_find);
							Inductor* in1 = dynamic_cast <Inductor*>(c_find);
							if (v1 != NULL)
							{
								if (pos_node == Nodelist[i]->getrank())
								{
									v += v1->getcomplex();
								}
								else
								{
									v -= v1->getcomplex();
								}
							}
							else if (r1 != NULL)
							{
								if (pos_node == Nodelist[i]->getrank())
								{
									v += r1->get_Impedance() * (Branchlist[counter]->getcurrent());
								}
								else
								{
									v -= r1->get_Impedance() * (Branchlist[counter]->getcurrent());
								}
							}
							else if (c1 != NULL)
							{
								if (pos_node == Nodelist[i]->getrank())
								{
									v += c1->get_Impedance() * (Branchlist[counter]->getcurrent());
								}
								else
								{
									v -= c1->get_Impedance() * (Branchlist[counter]->getcurrent());
								}
							}
							else if (in1 != NULL)
							{
								if (pos_node == Nodelist[i]->getrank())
								{
									v += in1->get_Impedance() * (Branchlist[counter]->getcurrent());
								}
								else
								{
									v -= in1->get_Impedance() * (Branchlist[counter]->getcurrent());
								}
							}
						}
						Nodelist[i]->setvoltage(v);

					}
				}
				else
				{
					for (int i = 1; i <= Nc1; i++)
					{
						complex<double> v = Nodelist[i - 1]->getvoltage();
						Component* c_find = NULL;
						int pos_node = -1;
						int neg_node = -1;
						for (int j = 0; j < comcount; j++)
						{
							int N1 = complist[j]->get_node1();
							int N2 = complist[j]->get_node2();
							if ((N1 == Nodelist[i]->getrank() || N2 == Nodelist[i]->getrank()) && (N1 == Nodelist[i - 1]->getrank() || N2 == Nodelist[i - 1]->getrank()))
							{
								c_find = complist[j];
								break;
							}
						}
						if (c_find != NULL)
						{
							pos_node = c_find->get_node1();
							neg_node = c_find->get_node2();
							Vsrc* v1 = dynamic_cast <Vsrc*>(c_find);
							Isrc* i1 = dynamic_cast <Isrc*>(c_find);

							Resistance* r1 = dynamic_cast <Resistance*>(c_find);
							Capacaitor* c1 = dynamic_cast <Capacaitor*>(c_find);
							Inductor* in1 = dynamic_cast <Inductor*>(c_find);
							if (v1 != NULL)
							{
								if (pos_node == Nodelist[i]->getrank())
								{
									v += v1->getcomplex();
								}
								else
								{
									v -= v1->getcomplex();
								}
							}
							else if (r1 != NULL)
							{
								if (pos_node == Nodelist[i]->getrank())
								{
									v += r1->get_Impedance() * (Branchlist[counter]->getcurrent());
								}
								else
								{
									v -= r1->get_Impedance() * (Branchlist[counter]->getcurrent());
								}
							}
							else if (c1 != NULL)
							{
								if (pos_node == Nodelist[i]->getrank())
								{
									v += c1->get_Impedance() * (Branchlist[counter]->getcurrent());
								}
								else
								{
									v -= c1->get_Impedance() * (Branchlist[counter]->getcurrent());
								}
							}
							else if (in1 != NULL)
							{
								if (pos_node == Nodelist[i]->getrank())
								{
									v += in1->get_Impedance() * (Branchlist[counter]->getcurrent());
								}
								else
								{
									v -= in1->get_Impedance() * (Branchlist[counter]->getcurrent());
								}
							}
						}
						Nodelist[i]->setvoltage(v);
					}
					for (int i = nodecount - 1; i >= Nc2; i--)
					{
						complex<double> v = (0, 0);
						Component* c_find = NULL;
						int pos_node = -1;
						int neg_node = -1;
						for (int j = 0; j < comcount; j++)
						{
							int N1 = complist[j]->get_node1();
							int N2 = complist[j]->get_node2();
							if ((N1 == Nodelist[i]->getrank() || N2 == Nodelist[i]->getrank()) && (N1 != Nodelist[i - 1]->getrank() && N2 != Nodelist[i - 1]->getrank()))
							{
								c_find = complist[j];
								if (N1 != Nodelist[i]->getrank())
								{
									v = Nodelist[N1]->getvoltage();
								}
								else if (N2 != Nodelist[i]->getrank())
								{
									v = Nodelist[N2]->getvoltage();
								}
								break;
							}
						}
						if (c_find != NULL)
						{
							pos_node = c_find->get_node1();
							neg_node = c_find->get_node2();
							Vsrc* v1 = dynamic_cast <Vsrc*>(c_find);
							Resistance* r1 = dynamic_cast <Resistance*>(c_find);
							Capacaitor* c1 = dynamic_cast <Capacaitor*>(c_find);
							Inductor* in1 = dynamic_cast <Inductor*>(c_find);
							if (v1 != NULL)
							{
								if (pos_node == Nodelist[i]->getrank())
								{
									v += v1->getcomplex();
								}
								else
								{
									v -= v1->getcomplex();
								}
							}
							else if (r1 != NULL)
							{
								if (pos_node == Nodelist[i]->getrank())
								{
									v += r1->get_Impedance() * (Branchlist[counter]->getcurrent());
								}
								else
								{
									v -= r1->get_Impedance() * (Branchlist[counter]->getcurrent());
								}
							}
							else if (c1 != NULL)
							{
								if (pos_node == Nodelist[i]->getrank())
								{
									v += c1->get_Impedance() * (Branchlist[counter]->getcurrent());
								}
								else
								{
									v -= c1->get_Impedance() * (Branchlist[counter]->getcurrent());
								}
							}
							else if (in1 != NULL)
							{
								if (pos_node == Nodelist[i]->getrank())
								{
									v += in1->get_Impedance() * (Branchlist[counter]->getcurrent());
								}
								else
								{
									v -= in1->get_Impedance() * (Branchlist[counter]->getcurrent());
								}
							}
						}
						Nodelist[i]->setvoltage(v);
					}
				}
			}
			else
			{
				for (int i = 1; i < nodecount; i++)
				{
					complex<double> v = Nodelist[i - 1]->getvoltage();
					Component* c_find = NULL;
					int pos_node = -1;
					int neg_node = -1;
					for (int j = 0; j < comcount; j++)
					{
						int N1 = complist[j]->get_node1();
						int N2 = complist[j]->get_node2();
						if ((N1 == Nodelist[i]->getrank() || N2 == Nodelist[i]->getrank()) && (N1 == Nodelist[i - 1]->getrank() || N2 == Nodelist[i - 1]->getrank()))
						{
							c_find = complist[j];
							break;
						}
					}
					if (c_find != NULL)
					{
						pos_node = c_find->get_node1();
						neg_node = c_find->get_node2();
						Vsrc* v1 = dynamic_cast <Vsrc*>(c_find);
						Isrc* i1 = dynamic_cast <Isrc*>(c_find);

						Resistance* r1 = dynamic_cast <Resistance*>(c_find);
						Capacaitor* c1 = dynamic_cast <Capacaitor*>(c_find);
						Inductor* in1 = dynamic_cast <Inductor*>(c_find);
						if (v1 != NULL)
						{
							if (pos_node == Nodelist[i]->getrank())
							{
								v += v1->getcomplex();
							}
							else
							{
								v -= v1->getcomplex();
							}
						}
						else if (r1 != NULL)
						{
							if (pos_node == Nodelist[i]->getrank())
							{
								v += r1->get_Impedance() * (Branchlist[counter]->getcurrent());
							}
							else
							{
								v -= r1->get_Impedance() * (Branchlist[counter]->getcurrent());
							}
						}
						else if (c1 != NULL)
						{
							if (pos_node == Nodelist[i]->getrank())
							{
								v += c1->get_Impedance() * (Branchlist[counter]->getcurrent());
							}
							else
							{
								v -= c1->get_Impedance() * (Branchlist[counter]->getcurrent());
							}
						}
						else if (in1 != NULL)
						{
							if (pos_node == Nodelist[i]->getrank())
							{
								v += in1->get_Impedance() * (Branchlist[counter]->getcurrent());
							}
							else
							{
								v -= in1->get_Impedance() * (Branchlist[counter]->getcurrent());
							}
						}
					}
					Nodelist[i]->setvoltage(v);
				}
			}
			counter++;
			//cout << Branchlist[counter]->getcurrent() << endl;
			for (int i = 0; i < nodecount; i++)
			{
				cout << "v"<<i <<"= "<< Nodelist[i]->getvoltage() << endl;
			}
			return 0;
		}
		///////////////// end of ciruts that has one loop ////////////////
		for (int i = 0; i < simple_nodecount; i++)
		{
			Node** connection = simple_Nodelist[i]->getNodeconnections();
			int N1 = connection[0]->getrank();        // the node that before the simplenode that we work on
			int N = simple_Nodelist[i]->getrank();   // the simplenode that we work on
			int N2 = connection[1]->getrank();       // the node that after the simplenode that we work on
			Branchlist[counter] = new Branch(Nodelist[N1], Nodelist[N2]);
			bool find = false;
			for (int j = 0; j < comcount; j++)
			{
				int N3 = complist[j]->get_node1();
				int N4 = complist[j]->get_node2();
				if ((N3 == N && N4 == N1) || (N3 == N1 && N4 == N) || (N3 == N && N4 == N2) || (N3 == N2 && N4 == N))
				{
					find = true;
				}


				if (find == true)
				{
					Vsrc* v1 = dynamic_cast <Vsrc*>(complist[j]);
					Isrc* i1 = dynamic_cast <Isrc*>(complist[j]);

					Resistance* r1 = dynamic_cast <Resistance*>(complist[j]);
					Capacaitor* c1 = dynamic_cast <Capacaitor*>(complist[j]);
					Inductor* in1 = dynamic_cast <Inductor*>(complist[j]);
					if (v1 != NULL)
					{
						Branchlist[counter]->setvolt(v1);
					}
					else if (i1 != NULL)
					{
						Branchlist[counter]->setcurentsource(i1);
					}
					else if (r1 != NULL)
					{
						Branchlist[counter]->setz(r1->get_Impedance());
					}
					else if (c1 != NULL)
					{
						Branchlist[counter]->setz(c1->get_Impedance());
					}
					else if (in1 != NULL)
					{
						Branchlist[counter]->setz(in1->get_Impedance());
					}

				}
				find = false;
			}

			// this code work when N2 is also a simple Node  
			// then we should add the component between N2 and the next Node to the same btanch
			// and Repeat this step if the Node that next to N2 is alse asimple
			// and so , if all nodes is asimple Node we must make exactly one branch
			// this branch will take all the components
			for (int k = i; k < simple_nodecount; k++)
			{
				N2;
				bool find2 = false;
				for (int k1 = i; k1 < simple_nodecount; k1++)
				{
					if (N2 == simple_Nodelist[k1]->getrank())
					{
						find2 = true;
					}
				}
				if (find2 == true)
				{
					i++;
					N = N2;
					connection = simple_Nodelist[i]->getNodeconnections();
					N2 = connection[1]->getrank();
					bool f = false;
					for (int k2 = 0; k2 < comcount; k2++)
					{
						int N3 = complist[k2]->get_node1();
						int N4 = complist[k2]->get_node2();
						if ((N3 == N && N4 == N2) || (N3 == N2 && N4 == N))
						{
							f = true;
						}
						if (f == true)
						{
							Vsrc* v1 = dynamic_cast <Vsrc*>(complist[k2]);
							Isrc* i1 = dynamic_cast <Isrc*>(complist[k2]);

							Resistance* r1 = dynamic_cast <Resistance*>(complist[k2]);
							Capacaitor* c1 = dynamic_cast <Capacaitor*>(complist[k2]);
							Inductor* in1 = dynamic_cast <Inductor*>(complist[k2]);
							if (v1 != NULL)
							{
								Branchlist[counter]->setvolt(v1);
							}
							else if (i1 != NULL)
							{
								Branchlist[counter]->setcurentsource(i1);
							}
							else if (r1 != NULL)
							{
								Branchlist[counter]->setz(r1->get_Impedance());
							}
							else if (c1 != NULL)
							{
								Branchlist[counter]->setz(c1->get_Impedance());
							}
							else if (in1 != NULL)
							{
								Branchlist[counter]->setz(in1->get_Impedance());
							}
						}
					}
				}
				else
				{}
			}
			counter++;
		}
	}
	/////////////////////////Convert Voltage Source to Current Source //////////////////////////////////////////////////////
	complex<double> zero;
	zero.real(0);
	zero.imag(0);
	Isrc* c;
	for (int i = 0; i < branchcount; i++)
	{
		Branch* b1 = Branchlist[i];
		if (Branchlist[i] != NULL)
		{
			if ((Branchlist[i]->getvolt() != NULL))
			{
				if (Branchlist[i]->getcsource() == NULL && Branchlist[i]->getz() != zero && Branchlist[i]->getvolt()->getcomplex() != zero)
				{
					int n = searcher(Branchlist[i]->getvolt()->get_name());
					c = simplification(Branchlist[i]->getvolt(), Branchlist[i]->getz(), Branchlist[i]->getnode1(), Branchlist[i]->getnode2());
					complist[comcount++] = c;
					Branchlist[i]->getvolt()->zerosetters();
				}
			}

		}
	}
	//////////////////////////////////////   Solving matrices of G & I & V   ////////////////////////////////////////////
	int num_non_sim_nodes = nonsimple_nodecount;
	int num = num_non_sim_nodes - 1;
	int* arr;
	arr = new int[nonsimple_nodecount - 1];
	for (int i = 1; i < nonsimple_nodecount; i++)
	{
		arr[i - 1] = nonsimple_Nodelist[i]->getrank();
	}
	Eigen::MatrixXcd m(num, num);
	for (int i = 0; i < num; i++)
	{
		for (int j = 0; j < num; j++)
		{
			m(i, j).real(0);
			m(i, j).imag(0);
		}
	}
	Eigen::MatrixXcd I(num, 1);
	for (int i = 0; i < num; i++)
	{
		I(i, 0) = 0;
	}
	Current_C_Current* cccs = NULL;
	Voltage_C_Voltage* vsvc = NULL;
	for (int i = 0; i < comcount; i++)
	{
		cccs = dynamic_cast<Current_C_Current*>(complist[i]);
		vsvc = dynamic_cast<Voltage_C_Voltage*>(complist[i]);
		if (cccs != NULL)
		{
			break;
		}
		if(vsvc !=NULL)
		{
			break;
		}
	}
	if (cccs != NULL)
	{
		for (int j = 0; j < comcount; j++)
		{
			if (cccs->get_Noded1() == complist[j]->get_node1() && cccs->get_Noded2() == complist[j]->get_node2()
				|| cccs->get_Noded2() == complist[j]->get_node1() && cccs->get_Noded1() == complist[j]->get_node2())
			{
				Resistance* r1 = dynamic_cast <Resistance*>(complist[j]);
				Capacaitor* c1 = dynamic_cast <Capacaitor*>(complist[j]);
				Inductor* in1 = dynamic_cast <Inductor*>(complist[j]);
				if (r1 != NULL)
				{
					cccs->set_Impedance(r1->get_Impedance());
				}
				else if (c1 != NULL)
				{
					cccs->set_Impedance(c1->get_Impedance());
				}
				else if (in1 != NULL)
				{
					cccs->set_Impedance(in1->get_Impedance());
				}

			}
		}
	}
	////////////////////////////////////////// Check Current-Current Source ////////////////////////////////////////////
	if (cccs != NULL)
	{
		int check_specialcase;
		int indexx;
		int no_Equations = num;
		Node* node_reference = NULL;
		for (int i = 0; i < branchcount; i++)
		{
			if (Branchlist[i]->getvolt() != NULL && Branchlist[i]->getz() == (0.0 + 0.0i))
			{
				if (Branchlist[i]->getnode1()->getrank() == arr[i] && Branchlist[i]->getnode2()->getrank() == 0)
				{
					no_Equations--;
					node_reference = Branchlist[i]->getnode1();
					node_reference->setvoltage(Branchlist[i]->getvolt()->getcomplex());
					check_specialcase = Branchlist[i]->getnode1()->getrank();
					break;
				}
				else if (Branchlist[i]->getnode2()->getrank() == arr[i] && Branchlist[i]->getnode1()->getrank() == 0)
				{
					no_Equations--;
					node_reference = Branchlist[i]->getnode2();
					node_reference->setvoltage(-Branchlist[i]->getvolt()->getcomplex());
					check_specialcase = Branchlist[i]->getnode2()->getrank();
					break;
				}
			}
		}
		////////////////////////////// Check Voltage Source connected alone to ground /////////////////////////////////////
		if (node_reference != NULL)
		{
			for (int k = 0; k < num; k++)
			{
				complex <double> Admittance_s(0, 0);
				for (int i = 0; i < branchcount; i++)
				{
					if ((Branchlist[i]->getnode1()->getrank() == arr[k]) || (Branchlist[i]->getnode2()->getrank() == arr[k]))
					{
						complex <double> r;
						r = Branchlist[i]->get_Admittance();
						Admittance_s += r;
					}
				}
				m(k, k) = Admittance_s;
			}
			for (int v = 0; v < num; v++)
			{
				for (int k = v + 1; k < num; k++)
				{
					complex <double> Admittance_d(0, 0);
					for (int l = 0; l < branchcount; l++)
					{
						if (((Branchlist[l]->getnode1()->getrank() == arr[v]) || (Branchlist[l]->getnode2()->getrank() == arr[v]))
							&& ((Branchlist[l]->getnode1()->getrank() == arr[k]) || (Branchlist[l]->getnode2()->getrank() == arr[k])))
						{
							Admittance_d += Branchlist[l]->get_Admittance();
						}
					}
					m(v, k) = -Admittance_d;
					m(k, v) = -Admittance_d;
				}
			}
			
			for (int i = 0; i < num; i++)
			{
				for (int k = 0; k < num; k++)
				{
					if (arr[i] == check_specialcase)
					{
						I(i, 0) = node_reference->getvoltage();
						m(i, k) = 0;
						if(i == k)
						{
							m(i, i) = 1;
						}
					}
				}
			}
			int cccs_node1;
			double sign = 1;
			int index;
			if (cccs != NULL)
			{
				for (int i = 1; i < nonsimple_nodecount; i++)
				{
					if (cccs->get_node1() == nonsimple_Nodelist[i]->getrank())
					{
						index = i;
						sign *= -1;
					}
					else if (cccs->get_node2() == nonsimple_Nodelist[i]->getrank())
					{
						index = i;
						sign *= 1;
					}

				}
				for (int i = 1; i < nonsimple_nodecount; i++)
				{
					if (nonsimple_Nodelist[i]->getrank() == cccs->get_Noded1())
					{
						m(index - 1, i - 1) += sign * cccs->get_Admittance() * cccs->get_cofficient();
					}
					else if (nonsimple_Nodelist[i]->getrank() == cccs->get_Noded2())
					{
						m(index - 1, i - 1) -= sign * cccs->get_Admittance() * cccs->get_cofficient();
					}
				}
			}
			MatrixXcd V = CalculateVoltNode(m, I); // function calculate volt of each node
			for (int i = 1; i < nonsimple_nodecount; i++)
			{
				nonsimple_Nodelist[i]->setvoltage(V(i - 1, 0));
			}
			for (int i = 0; i < branchcount; i++)
			{
				Branchlist[i]->Calculatecurrent();
			}
			cout << m;
			cout << "..........................." << endl;
			cout << I << endl;
			cout << "..........................." << endl;
			cout << V << endl;
			cout << "..........................." << endl;
			Save(nonsimple_Nodelist, Branchlist, nonsimple_nodecount, branchcount);
			return 0;
		}
		else
		{
		for (int k = 0; k < num; k++)
		{
			complex <double> Admittance_s(0, 0);
			for (int i = 0; i < branchcount; i++)
			{
				if ((Branchlist[i]->getnode1()->getrank() == arr[k]) || (Branchlist[i]->getnode2()->getrank() == arr[k]))
				{
					complex <double> r;
					r = Branchlist[i]->get_Admittance();
					Admittance_s += r;
				}
			}
			m(k, k) = Admittance_s;
		}
		for (int v = 0; v < num; v++)
		{
			for (int k = v + 1; k < num; k++)
			{
				complex <double> Admittance_d(0, 0);
				for (int l = 0; l < branchcount; l++)
				{
					if (((Branchlist[l]->getnode1()->getrank() == arr[v]) || (Branchlist[l]->getnode2()->getrank() == arr[v]))
						&& ((Branchlist[l]->getnode1()->getrank() == arr[k]) || (Branchlist[l]->getnode2()->getrank() == arr[k])))
					{
						Admittance_d += Branchlist[l]->get_Admittance();
					}
				}
				m(v, k) = -Admittance_d;
				m(k, v) = -Admittance_d;
			}
		}
			for (int j = 0; j < num; j++)
			{
				for (int i = 0; i < comcount; i++)
				{
					Component* comp = complist[i];
					Isrc* current = dynamic_cast<Isrc*>(comp);
					if (current != NULL)
					{	if (current->get_node1() == arr[j])
						{
							complex <double> curr = current->getcomplex();
							I(j, 0) += curr;
	
						}
						if (current->get_node2() == arr[j])
						{
							complex <double> curr = -current->getcomplex();
							I(j, 0) += curr;
						}
					}
				}
			}
		int cccs_node1;
		double sign = 1;
		int index;
			if (cccs != NULL)
			{
				for (int i = 1; i < nonsimple_nodecount; i++)
				{
					if (cccs->get_node1() == nonsimple_Nodelist[i]->getrank())
					{
						index = i;
						sign *= -1;
					}
					else if (cccs->get_node2() == nonsimple_Nodelist[i]->getrank())
					{
						index = i;
						sign *= 1;
					}

				}
				for (int i = 1; i < nonsimple_nodecount; i++)
				{
					if (nonsimple_Nodelist[i]->getrank() == cccs->get_Noded1())
					{
						m(index - 1, i - 1) += sign * cccs->get_Admittance() * cccs->get_cofficient();
					}
					else if (nonsimple_Nodelist[i]->getrank() == cccs->get_Noded2())
					{
						m(index - 1, i - 1) -= sign * cccs->get_Admittance() * cccs->get_cofficient();
					}
				}
			}
			MatrixXcd V = CalculateVoltNode(m, I); // function calculate volt of each node
			for (int i = 1; i < nonsimple_nodecount; i++)
			{
				nonsimple_Nodelist[i]->setvoltage(V(i - 1, 0));
			}
			for (int i = 0; i < branchcount; i++)
			{
				Branchlist[i]->Calculatecurrent();
			}
			cout << "..........................." << endl;
			cout << m << endl;
			cout << "..........................." << endl;
			cout << I << endl;
			cout << "..........................." << endl;
			cout << V << endl;
			Save(nonsimple_Nodelist, Branchlist, nonsimple_nodecount, branchcount);
			return 0;

		}
	}
	////////////////////////////////////////// check if Volt - Volt source ////////////////////////////////////
else if (vsvc != NULL)
	{
	/////////////////// super node //////////////////////////////
	bool R1 = false, R2 = false;
	int reference1, reference2;
	Vsrc* voltage_source = NULL;
	Vsrc* voltage_source2 = NULL;

	for (int i = 0; i < comcount; i++)
	{
		voltage_source = dynamic_cast<Vsrc*> (complist[i]);
		if (voltage_source != NULL)
		{
			for (int j = 0; j < num; j++)
			{
				if (voltage_source->get_node1() == arr[j])
				{
					voltage_source2 = voltage_source;
					reference1 = j;
					R1 = true;
				}
				if (voltage_source->get_node2() == arr[j])
				{
					reference2 = j;
					R2 = true;
				}
			}
		}
	}
	Voltage_C_Voltage* vv = vsvc;
	int pos_node = vv->get_node1();
	int cof = vv->get_cofficient();
	int neg_node = vv->get_node2();
	int node_d_pos = vv->get_Noded1();
	int node_d_neg = vv->get_Noded2();
	int node_v_d;
	int node_vv_d;
	int node_s;
	if ((vv->get_node1() == voltage_source2->get_node1()) || (vv->get_node1() == voltage_source2->get_node2()))
	{
		node_s = vv->get_node1();
		if (vv->get_node1() == voltage_source2->get_node1())
		{
			node_v_d = voltage_source2->get_node2();
		}
		else
		{
			node_v_d = voltage_source2->get_node1();
		}
		node_vv_d = vv->get_node2();
	}
	else if ((vv->get_node2() == voltage_source2->get_node1()) || (vv->get_node2() == voltage_source2->get_node2()))
	{
		node_s = vv->get_node2();
		if (vv->get_node2() == voltage_source2->get_node1())
		{
			node_v_d = voltage_source2->get_node2();
		}
		else
		{
			node_v_d = voltage_source2->get_node1();
		}
		node_vv_d = vv->get_node1();
	}
	if ((R1 && R2) && (vsvc->get_node1() != 0 && vsvc->get_node2() != 0))
	{
		for (int k = 0; k < num; k++)
		{
			for (int i = 0; i < nonsimple_nodecount; i++)
			{
				if (voltage_source2->get_node1() == nonsimple_Nodelist[i]->getrank())
				{
					m(node_v_d - 1, reference1) = 1;
				}
				else if (voltage_source2->get_node2() == nonsimple_Nodelist[i]->getrank())
				{
					m(node_v_d - 1, reference2) = -1;
				}
				else
				{
					m(node_v_d - 1, k) = 0;
				}
			}
			I(node_v_d - 1, 0) = voltage_source2->getcomplex();
		}///////////////
		m(node_vv_d - 1, node_d_pos - 1) = 0;
		m(node_vv_d - 1, node_d_neg - 1) = 0;
		m(node_vv_d - 1, pos_node - 1) = 0;
		m(node_vv_d - 1, neg_node - 1) = 0;
		m(node_vv_d - 1, pos_node - 1) = 1;
		m(node_vv_d - 1, neg_node - 1) = -1;
		m(node_vv_d - 1, node_d_pos - 1) += -cof;
		m(node_vv_d - 1, node_d_neg - 1) += cof;
		I(node_vv_d - 1, 0) = 0;
		I(node_vv_d - 1, 0) = 0;
		for (int k = 0; k < branchcount; k++)
		{
			if ((((Branchlist[k]->getnode1()->getrank() == node_vv_d) && (Branchlist[k]->getnode2()->getrank() == 0))
				|| ((Branchlist[k]->getnode1()->getrank() == 0) && (Branchlist[k]->getnode2()->getrank() == node_vv_d)))
				|| (((Branchlist[k]->getnode1()->getrank() == node_v_d) && (Branchlist[k]->getnode2()->getrank() == 0))
					|| ((Branchlist[k]->getnode1()->getrank() == 0) && (Branchlist[k]->getnode2()->getrank() == node_v_d)))
				|| (((Branchlist[k]->getnode1()->getrank() == node_s) && (Branchlist[k]->getnode2()->getrank() == 0))
					|| ((Branchlist[k]->getnode1()->getrank() == 0) && (Branchlist[k]->getnode2()->getrank() == node_s))))
			{
				Isrc* currentsource = Branchlist[k]->getcsource();
				if (currentsource != NULL)
				{
					I(node_s - 1, 0) += Branchlist[k]->getcsource()->getcomplex();
				}
			}
		}
		complex <double> Admittance_d4(0, 0);
		for (int k = 0; k < branchcount; k++)
		{
			if (((Branchlist[k]->getnode1()->getrank() == node_vv_d) && (Branchlist[k]->getnode2()->getrank() == 0))
				|| ((Branchlist[k]->getnode1()->getrank() == 0) && (Branchlist[k]->getnode2()->getrank() == node_vv_d)))
			{
				m(node_s - 1, node_vv_d - 1) += Branchlist[k]->get_Admittance();
			}
			if (((Branchlist[k]->getnode1()->getrank() == node_v_d) && (Branchlist[k]->getnode2()->getrank() == 0))
				|| ((Branchlist[k]->getnode1()->getrank() == 0) && (Branchlist[k]->getnode2()->getrank() == node_v_d)))
			{
				m(node_s - 1, node_v_d - 1) += Branchlist[k]->get_Admittance();
			}
			if (((Branchlist[k]->getnode1()->getrank() == node_s) && (Branchlist[k]->getnode2()->getrank() == 0))
				|| ((Branchlist[k]->getnode1()->getrank() == 0) && (Branchlist[k]->getnode2()->getrank() == node_s)))
			{
				m(node_s - 1, node_s - 1) += Branchlist[k]->get_Admittance();
			}
		}
		MatrixXcd V = CalculateVoltNode(m, I); // function calculate volt of each node
		for (int i = 1; i < nonsimple_nodecount; i++)
		{
			nonsimple_Nodelist[i]->setvoltage(V(i - 1, 0));
		}
		for (int i = 0; i < branchcount; i++)
		{
			Branchlist[i]->Calculatecurrent();
		}
		cout << "..........................." << endl;
		cout << m << endl;
		cout << "..........................." << endl;
		cout << I << endl;
		cout << "..........................." << endl;
		cout << V << endl;
		Save(nonsimple_Nodelist, Branchlist, nonsimple_nodecount, branchcount);
		return 0;
	}
	///////////////////super node ///////////////////////////
	int check_specialcase;
	int no_Equations = num;
	Node* node_reference = NULL;
	for (int i = 0; i < branchcount; i++)
	{
		if (Branchlist[i]->getvolt() != NULL && Branchlist[i]->getz() == (0.0 + 0.0i))
		{
			if (Branchlist[i]->getnode1()->getrank() == arr[i] && Branchlist[i]->getnode2()->getrank() == 0)
			{
				no_Equations--;
				node_reference = Branchlist[i]->getnode1();
				node_reference->setvoltage(Branchlist[i]->getvolt()->getcomplex());
				check_specialcase = Branchlist[i]->getnode1()->getrank();
				break;
			}
			else if (Branchlist[i]->getnode2()->getrank() == arr[i] && Branchlist[i]->getnode1()->getrank() ==0 )
			{
				no_Equations--;
				node_reference = Branchlist[i]->getnode2();
				node_reference->setvoltage(-Branchlist[i]->getvolt()->getcomplex());
				check_specialcase = Branchlist[i]->getnode2()->getrank();
				break;
			}
		}
	}
	/////////////////////////////////// check if voltage source connected to ground ///////////////////////////////// 
	if (node_reference != NULL)
	{
		for (int k = 0; k < num; k++)
		{
			complex <double> Admittance_s(0, 0);
			for (int i = 0; i < branchcount; i++)
			{
				if ((Branchlist[i]->getnode1()->getrank() == arr[k]) || (Branchlist[i]->getnode2()->getrank() == arr[k]))
				{
					complex <double> r;
					r = Branchlist[i]->get_Admittance();
					Admittance_s += r;
				}
			}
			m(k, k) = Admittance_s;
		}
		for (int v = 0; v < num; v++)
		{
			for (int k = v + 1; k < num; k++)
			{
				complex <double> Admittance_d(0, 0);
				for (int l = 0; l < branchcount; l++)
				{
					if (((Branchlist[l]->getnode1()->getrank() == arr[v]) || (Branchlist[l]->getnode2()->getrank() == arr[v]))
						&& ((Branchlist[l]->getnode1()->getrank() == arr[k]) || (Branchlist[l]->getnode2()->getrank() == arr[k])))
					{
						Admittance_d += Branchlist[l]->get_Admittance();
					}
				}
				m(v, k) = -Admittance_d;
				m(k, v) = -Admittance_d;
			}
		}
		for (int i = 0; i < num; i++)
		{
			for (int k = 0; k < num; k++)
			{
				if (arr[i] == check_specialcase)
				{
					I(i, 0) = node_reference->getvoltage();
					m(i, k) = 0;
					if (i == k)
					{
						m(i, i) = 1;
					}
				}
			}
		}
		for (int i = 0; i < comcount; i++)
		{
			Voltage_C_Voltage* vv = dynamic_cast<Voltage_C_Voltage*>(complist[i]);
			if (vv != NULL)
			{
				int pos_node = vv->get_node1();
				if (pos_node == 0)
				{
					int cof = -vv->get_cofficient();
					int node_d_pos = vv->get_Noded1();
					int node_d_neg = vv->get_Noded2();
					int node_neg = vv->get_node2();
					for (int i = 0; i < num; i++)
					{
						m(node_neg - 1, i) = 0;
					}
					m(node_neg - 1, node_d_pos - 1) += cof;
					m(node_neg - 1, node_d_neg - 1) += -cof;
					m(node_neg - 1, node_neg - 1) += 1;
				}
				else if (vv->get_node2() == 0)
				{
					int cof = vv->get_cofficient();
					int node_d_pos = vv->get_Noded1();
					int node_d_neg = vv->get_Noded2();
					int node_neg = vv->get_node2();
					for (int i = 0; i < num; i++)
					{
						m(pos_node - 1, i) = 0;
					}
					m(pos_node - 1, node_d_pos - 1) += -cof;
					m(pos_node - 1, node_d_neg - 1) += cof;
					m(pos_node - 1, pos_node - 1) += 1;
				}
				else
				{
					int cof = vv->get_cofficient();
					int neg_node = vv->get_node2();
					int node_d_pos = vv->get_Noded1();
					int node_d_neg = vv->get_Noded2();
					m(pos_node - 1, node_d_pos - 1) = 0;
					m(pos_node - 1, node_d_neg - 1) = 0;
					m(pos_node - 1, pos_node - 1) = 0;
					m(pos_node - 1, neg_node - 1) = 0;
					m(pos_node - 1, pos_node - 1) = 1;
					m(pos_node - 1, neg_node - 1) = -1;
					m(pos_node - 1, node_d_pos - 1) += -cof;
					m(pos_node - 1, node_d_neg - 1) += cof;
					I(pos_node - 1, 0) = 0;
					I(neg_node - 1, 0) = 0;
					for (int k = 0; k < branchcount; k++)
					{
						if (((Branchlist[k]->getnode1()->getrank() == pos_node) || (Branchlist[k]->getnode2()->getrank() == pos_node))
							&& ((Branchlist[k]->getnode1()->getrank() != neg_node) && (Branchlist[k]->getnode2()->getrank() != neg_node)))
						{
							Isrc* currentsource = Branchlist[k]->getcsource();
							if (currentsource != NULL)
							{
								I(neg_node - 1, 0) += Branchlist[k]->getcsource()->getcomplex();
							}
						}
					}
					for (int k = 0; k < branchcount; k++)
					{
						if (((Branchlist[k]->getnode1()->getrank() == neg_node) || (Branchlist[k]->getnode2()->getrank() == neg_node))
							&& ((Branchlist[k]->getnode1()->getrank() != pos_node) && (Branchlist[k]->getnode2()->getrank() != pos_node)))
						{
							Isrc* currentsource = Branchlist[k]->getcsource();
							if (currentsource != NULL)
							{
								I(neg_node - 1, 0) += Branchlist[k]->getcsource()->getcomplex();
							}
						}
					}
					complex <double> Admittance_d(0, 0);
					for (int k = 0; k < branchcount; k++)
					{

						if (((Branchlist[k]->getnode1()->getrank() == pos_node) || (Branchlist[k]->getnode2()->getrank() == pos_node))
							&& ((Branchlist[k]->getnode1()->getrank() != neg_node) && (Branchlist[k]->getnode2()->getrank() != neg_node)))
						{
							Admittance_d += Branchlist[k]->get_Admittance();
						}
					}
					m(neg_node - 1, pos_node - 1) = Admittance_d;
					complex <double> Admittance_d2(0, 0);
					for (int k = 0; k < branchcount; k++)
					{

						if (((Branchlist[k]->getnode1()->getrank() == neg_node) || (Branchlist[k]->getnode2()->getrank() == neg_node))
							&& ((Branchlist[k]->getnode1()->getrank() != pos_node) && (Branchlist[k]->getnode2()->getrank() != pos_node)))
						{
							Admittance_d2 += Branchlist[k]->get_Admittance();
						}
					}
					m(neg_node - 1, neg_node - 1) = Admittance_d2;
					Admittance_d2 = 0.0 + 0.0i;
					complex <double> Admittance_d3(0, 0);
					for (int i = 0; i < num; i++)
					{
						if (i != neg_node - 1 && i != pos_node - 1)
						{
							for (int k = 0; k < branchcount; k++)
							{
								if (Branchlist[k]->getnode1()->getrank() == arr[i] && Branchlist[k]->getnode2()->getrank() == pos_node ||
									Branchlist[k]->getnode2()->getrank() == arr[i] && Branchlist[k]->getnode1()->getrank() == pos_node)
								{
									Admittance_d2 += Branchlist[k]->get_Admittance();
								}
							}
							for (int k = 0; k < branchcount; k++)
							{
								if (Branchlist[k]->getnode1()->getrank() == arr[i] && Branchlist[k]->getnode2()->getrank() == neg_node ||
									Branchlist[k]->getnode2()->getrank() == arr[i] && Branchlist[k]->getnode1()->getrank() == neg_node)
								{
									Admittance_d3 += Branchlist[k]->get_Admittance();
								}
							}
							m(neg_node - 1, i) = -Admittance_d2 - Admittance_d3;
						}

					}

				}
			}
		}
		MatrixXcd V = CalculateVoltNode(m, I); // function calculate volt of each node
		for (int i = 1; i < nonsimple_nodecount; i++)
		{
			nonsimple_Nodelist[i]->setvoltage(V(i - 1, 0));
		}
		for (int i = 0; i < branchcount; i++)
		{
			Branchlist[i]->Calculatecurrent();
		}
		cout << "..........................." << endl;
		cout << m << endl;
		cout << "..........................." << endl;
		cout << I << endl;
		cout << "..........................." << endl;
		cout << V << endl;
		Save(nonsimple_Nodelist, Branchlist, nonsimple_nodecount, branchcount);
		return 0;
	}
	else if (node_reference == NULL)
	{
		for (int i = 0; i < comcount; i++)
		{
			Voltage_C_Voltage* vv = dynamic_cast<Voltage_C_Voltage*>(complist[i]);
			if (vv != NULL)
			{
				// node that is not equal to zero
				int pos_node = vv->get_node1();
				if (pos_node == 0)
				{
					int cof = -vv->get_cofficient();
					int node_d_pos = vv->get_Noded1();
					int node_d_neg = vv->get_Noded2();
					int node_neg = vv->get_node2();
					for (int i = 0; i < num; i++)
					{
						m(node_neg - 1, i) = 0;
					}
					m(node_neg - 1, node_d_pos - 1) += cof;
					m(node_neg - 1, node_d_neg - 1) += -cof;
					m(node_neg - 1, node_neg - 1) += 1;
				}
				else if (vv->get_node2() == 0)
				{
					int cof = vv->get_cofficient();
					int node_d_pos = vv->get_Noded1();
					int node_d_neg = vv->get_Noded2();
					int node_neg = vv->get_node2();
					for (int i = 0; i < num; i++)
					{
						m(pos_node - 1, i) = 0;
					}
					m(pos_node - 1, node_d_pos - 1) += -cof;
					m(pos_node - 1, node_d_neg - 1) += cof;
					m(pos_node - 1, pos_node - 1) += 1;
				}
				else
				{
					int cof = vv->get_cofficient();
					int neg_node = vv->get_node2();
					int node_d_pos = vv->get_Noded1();
					int node_d_neg = vv->get_Noded2();
					m(pos_node - 1, node_d_pos - 1) = 0;
					m(pos_node - 1, node_d_neg - 1) = 0;
					m(pos_node - 1, pos_node - 1) = 0;
					m(pos_node - 1, neg_node - 1) = 0;
					m(pos_node - 1, pos_node - 1) = 1;
					m(pos_node - 1, neg_node - 1) = -1;
					m(pos_node - 1, node_d_pos - 1) += -cof;
					m(pos_node - 1, node_d_neg - 1) += cof;
					I(pos_node - 1, 0) = 0;
					I(neg_node - 1, 0) = 0;
					for (int k = 0; k < branchcount; k++)
					{
						if (((Branchlist[k]->getnode1()->getrank() == pos_node) || (Branchlist[k]->getnode2()->getrank() == pos_node))
							&& ((Branchlist[k]->getnode1()->getrank() != neg_node) && (Branchlist[k]->getnode2()->getrank() != neg_node)))
						{
							Isrc* currentsource = Branchlist[k]->getcsource();
							if (currentsource != NULL)
							{
								I(neg_node - 1, 0) += Branchlist[k]->getcsource()->getcomplex();
							}
						}
					}
					for (int k = 0; k < branchcount; k++)
					{
						if (((Branchlist[k]->getnode1()->getrank() == neg_node) || (Branchlist[k]->getnode2()->getrank() == neg_node))
							&& ((Branchlist[k]->getnode1()->getrank() != pos_node) && (Branchlist[k]->getnode2()->getrank() != pos_node)))
						{
							Isrc* currentsource = Branchlist[k]->getcsource();
							if (currentsource != NULL)
							{
								I(neg_node - 1, 0) += Branchlist[k]->getcsource()->getcomplex();
							}
						}
					}
					complex <double> Admittance_d(0, 0);
					for (int k = 0; k < branchcount; k++)
					{

						if (((Branchlist[k]->getnode1()->getrank() == pos_node) || (Branchlist[k]->getnode2()->getrank() == pos_node))
							&& ((Branchlist[k]->getnode1()->getrank() != neg_node) && (Branchlist[k]->getnode2()->getrank() != neg_node)))
						{
							Admittance_d += Branchlist[k]->get_Admittance();
						}
					}
					m(neg_node - 1, pos_node - 1) = Admittance_d;
					complex <double> Admittance_d2(0, 0);
					for (int k = 0; k < branchcount; k++)
					{

						if (((Branchlist[k]->getnode1()->getrank() == neg_node) || (Branchlist[k]->getnode2()->getrank() == neg_node))
							&& ((Branchlist[k]->getnode1()->getrank() != pos_node) && (Branchlist[k]->getnode2()->getrank() != pos_node)))
						{
							Admittance_d2 += Branchlist[k]->get_Admittance();
						}
					}
					m(neg_node - 1, neg_node - 1) = Admittance_d2;
					Admittance_d2 = 0.0 + 0.0i;
					complex <double> Admittance_d3(0, 0);
					for (int i = 0; i < num; i++)
					{
						if (i != neg_node - 1 && i != pos_node - 1)
						{
							for (int k = 0; k < branchcount; k++)
							{
								if (Branchlist[k]->getnode1()->getrank() == arr[i] && Branchlist[k]->getnode2()->getrank() == pos_node ||
									Branchlist[k]->getnode2()->getrank() == arr[i] && Branchlist[k]->getnode1()->getrank() == pos_node)
								{
									Admittance_d2 += Branchlist[k]->get_Admittance();
								}
							}
							for (int k = 0; k < branchcount; k++)
							{
								if (Branchlist[k]->getnode1()->getrank() == arr[i] && Branchlist[k]->getnode2()->getrank() == neg_node ||
									Branchlist[k]->getnode2()->getrank() == arr[i] && Branchlist[k]->getnode1()->getrank() == neg_node)
								{
									Admittance_d3 += Branchlist[k]->get_Admittance();
								}
							}
							m(neg_node - 1, i) = -Admittance_d2 - Admittance_d3;
						}

					}

				}
			}
		}
		cout << "................" << endl;
		cout << m << endl;
		cout << "................" << endl;
		cout << I << endl;
		MatrixXcd V = CalculateVoltNode(m, I); // function calculate volt of each node
		for (int i = 1; i < nonsimple_nodecount; i++)
		{
			nonsimple_Nodelist[i]->setvoltage(V(i - 1, 0));
		}
		for (int i = 0; i < branchcount; i++)
		{
			Branchlist[i]->Calculatecurrent();
		}
		cout << "the volt at Each node " << endl;
		cout << V << endl;
		Save(nonsimple_Nodelist, Branchlist, nonsimple_nodecount, branchcount);
		return 0;
	}
	}
	else
	{
/////////////////////////////////////////////// Voltage Source Connected to ground //////////////////////////////////////////
		int check_specialcase;
		int no_Equations = num;
		Node* node_reference = NULL;
		for (int i = 0; i < branchcount; i++)
		{
			if (Branchlist[i]->getvolt() != NULL && Branchlist[i]->getz() == (0.0 + 0.0i))
			{
				if (Branchlist[i]->getnode1()->getrank() == arr[i] && Branchlist[i]->getnode2()->getrank() == 0)
				{
					no_Equations--;
					node_reference = Branchlist[i]->getnode1();
					node_reference->setvoltage(Branchlist[i]->getvolt()->getcomplex());
					check_specialcase = Branchlist[i]->getnode1()->getrank();
					break;
				}
				else if (Branchlist[i]->getnode2()->getrank() == arr[i] && Branchlist[i]->getnode1()->getrank() == 0)
				{
					no_Equations--;
					node_reference = Branchlist[i]->getnode2();
					node_reference->setvoltage(-Branchlist[i]->getvolt()->getcomplex());
					check_specialcase = Branchlist[i]->getnode2()->getrank();
					break;
				}
			}
		}
		Eigen::MatrixXcd m1(no_Equations, no_Equations);
		for (int i = 0; i < no_Equations; i++)
		{
			for (int j = 0; j < no_Equations; j++)
			{
				m1(i, j).real(0);
				m1(i, j).imag(0);
			}
		}
		Eigen::MatrixXcd I1(no_Equations, 1);
		for (int i = 0; i < no_Equations; i++)
		{
			I1(i, 0) = 0;
		}
		complex<double>* admitt = new complex<double>[no_Equations];
		for (int l = 0; l < no_Equations; l++)
		{
			admitt[l] = 0.0 + 0.0i;
		}
		if (node_reference != NULL)
		{
			int* arr2 = new int[no_Equations];
			int k = 0;
			for (int i = 0; i < num; i++)
			{
				if (arr[i] != check_specialcase)
				{
					arr2[k] = arr[i];
					k++;
				}

			}
			for (int i = 0; i < no_Equations; i++)
			{
				for (int k = 0; k < branchcount; k++)
				{
					if ((Branchlist[k]->getnode1()->getrank() == check_specialcase ||
						Branchlist[k]->getnode2()->getrank() == check_specialcase) &&
						(Branchlist[k]->getnode1()->getrank() == arr2[i] ||
							Branchlist[k]->getnode2()->getrank() == arr2[i]))
					{
						admitt[i] += Branchlist[k]->get_Admittance();
					}
				}
			}
			for (int i = 0; i < no_Equations; i++)
			{
				I1(i, 0) += node_reference->getvoltage() * admitt[i];
			}

			for (int k = 0; k < no_Equations; k++)
			{
				complex <double> Admittance_s(0, 0);
				for (int i = 0; i < branchcount; i++)
				{
					if ((Branchlist[i]->getnode1()->getrank() == arr2[k]) || (Branchlist[i]->getnode2()->getrank() == arr2[k]))
					{
						complex <double> r;
						r = Branchlist[i]->get_Admittance();
						Admittance_s += r;
					}
				}
				m1(k, k) = Admittance_s;
			}
			for (int v = 0; v < no_Equations; v++)
			{
				for (int k = v + 1; k < no_Equations; k++)
				{
					complex <double> Admittance_d(0, 0);
					for (int l = 0; l < branchcount; l++)
					{
						if (((Branchlist[l]->getnode1()->getrank() == arr2[v]) || (Branchlist[l]->getnode2()->getrank() == arr2[v]))
							&& ((Branchlist[l]->getnode1()->getrank() == arr2[k]) || (Branchlist[l]->getnode2()->getrank() == arr2[k])))
						{
							Admittance_d += Branchlist[l]->get_Admittance();
						}
					}
					m1(v, k) = -Admittance_d;
					m1(k, v) = -Admittance_d;
				}
			}

			for (int j = 0; j < no_Equations; j++)
			{
				for (int i = 0; i < comcount; i++)
				{
					Component* comp = complist[i];
					Isrc* current = dynamic_cast<Isrc*>(comp);
					if (current != NULL)
					{
						if (current->get_node1() == arr2[j])
						{
							complex <double> curr = current->getcomplex();
							I1(j, 0) += curr;

						}
						if (current->get_node2() == arr2[j])
						{
							complex <double> curr = -current->getcomplex();
							I1(j, 0) += curr;
						}
					}
				}
			}
			cout << "................" << endl;
			cout << m1 << endl;
			cout << "................" << endl;
			cout << I1 << endl;
			cout << "................" << endl;
			MatrixXcd V1 = CalculateVoltNode(m1, I1); // function calculate volt of each node
			k = 0;
			for (int i = 1; i < nonsimple_nodecount; i++)
			{
				if (nonsimple_Nodelist[i]->getrank() != check_specialcase)
				{
					nonsimple_Nodelist[i]->setvoltage(V1(k, 0));
					k++;
				}
			}
			for (int i = 0; i < branchcount; i++)
			{
				Branchlist[i]->Calculatecurrent();
				cout << "current " << i << " :" << Branchlist[i]->getcurrent();
			}
			cout << "................" << endl;
			cout << V1;
			cout << "................" << endl;
			Save(nonsimple_Nodelist, Branchlist, nonsimple_nodecount, branchcount);
			return 0;
		}
////////////////////////      Super Node independent Sources /////////////////////////////////////////////////////////
		bool R1 = false, R2 = false;
		int reference1, reference2;
		Vsrc* voltage_source = NULL;
		Vsrc* voltage_source2 = NULL;

		for (int i = 0; i < comcount; i++)
		{
			voltage_source = dynamic_cast<Vsrc*> (complist[i]);
			if (voltage_source != NULL)
			{
				for (int j = 0; j < num; j++)
				{
					if (voltage_source->get_node1() == arr[j])
					{
						voltage_source2 = voltage_source;
						reference1 = j;
						R1 = true;
					}
					if (voltage_source->get_node2() == arr[j])
					{
						reference2 = j;
						R2 = true;
					}
				}
			}
		}
		if (R1 && R2)
		{
			for (int k = 0; k < num; k++)
			{
				for (int i = 0; i < nonsimple_nodecount; i++)
				{
					if (voltage_source2->get_node1() == nonsimple_Nodelist[i]->getrank())
					{
						m(0, reference1) = 1;
					}
					else if (voltage_source2->get_node2() == nonsimple_Nodelist[i]->getrank())
					{
						m(0, reference2) = -1;
					}
					else
					{
						m(0, k) = 0;
					}
				}
				I(0, 0) = voltage_source2->getcomplex();
			}///////////////
			cout << m << endl;
			complex <double> Admittance_d(0, 0);
			for (int k = 0; k < branchcount; k++)
			{

				if (((Branchlist[k]->getnode1()->getrank() == arr[reference1]) || (Branchlist[k]->getnode2()->getrank() == arr[reference1]))
					&& ((Branchlist[k]->getnode1()->getrank() != arr[reference2]) && (Branchlist[k]->getnode2()->getrank() != arr[reference2])))
				{
					Admittance_d += Branchlist[k]->get_Admittance();
				}
			}
			m(1, reference1) = Admittance_d;
			complex <double> Admittance_d2(0, 0);
			for (int k = 0; k < branchcount; k++)
			{

				if (((Branchlist[k]->getnode1()->getrank() == arr[reference2]) || (Branchlist[k]->getnode2()->getrank() == arr[reference2]))
					&& ((Branchlist[k]->getnode1()->getrank() != arr[reference1]) && (Branchlist[k]->getnode2()->getrank() != arr[reference1])))
				{
					Admittance_d2 += Branchlist[k]->get_Admittance();
				}
			}
			m(1, reference2) = Admittance_d2;
			Admittance_d2 = 0.0 + 0.0i;
			complex <double> Admittance_d3(0, 0);
			for (int i = 0; i < num; i++)
			{
				if (i != reference1 && i != reference2)
				{
					for (int k = 0; k < branchcount; k++)
					{
						if (Branchlist[k]->getnode1()->getrank() == arr[i] && Branchlist[k]->getnode2()->getrank() == arr[reference1] ||
							Branchlist[k]->getnode2()->getrank() == arr[i] && Branchlist[k]->getnode1()->getrank() == arr[reference1])
						{
							Admittance_d2 += Branchlist[k]->get_Admittance();
						}
					}
					for (int k = 0; k < branchcount; k++)
					{
						if (Branchlist[k]->getnode1()->getrank() == arr[i] && Branchlist[k]->getnode2()->getrank() == arr[reference2] ||
							Branchlist[k]->getnode2()->getrank() == arr[i] && Branchlist[k]->getnode1()->getrank() == arr[reference2])
						{
							Admittance_d3 += Branchlist[k]->get_Admittance();
						}
					}
					m(1, i) = -Admittance_d2 - Admittance_d3;
				}

			}

			for (int i = 2; i < num; i++)
			{
				for (int j = 0; j < num; j++)
				{
					if (arr[j] == voltage_source->get_node2() || arr[j] == voltage_source->get_node1())
					{
					}
					else
					{
						complex <double> Admittance_s(0, 0);
						for (int k = 0; k < branchcount; k++)
						{
							if ((Branchlist[i]->getnode1()->getrank() == arr[j]) || (Branchlist[i]->getnode2()->getrank() == arr[j]))
							{
								complex <double> r;
								r = Branchlist[i]->get_Admittance();
								Admittance_s += r;
								if (Branchlist[i]->getnode1()->getrank() == arr[reference1] ||
									Branchlist[i]->getnode2()->getrank() == arr[reference1])
								{
									Admittance_d = 0.0 + 0.0i;
									Admittance_d = Branchlist[i]->get_Admittance();
								}
								if (Branchlist[i]->getnode1()->getrank() == arr[reference2] ||
									Branchlist[i]->getnode2()->getrank() == arr[reference2])
								{
									Admittance_d2 = 0.0 + 0.0i;
									Admittance_d2 = Branchlist[i]->get_Admittance();
								}
							}
						}
						m(i, j) = Admittance_s;
						m(i, reference1) = -Admittance_d;
						m(i, reference1) = -Admittance_d2;
					}
				}
			}
			////////////////// Calculate Currents ///////////////////////////////
			for (int j = 1; j < num; j++)
			{
				for (int i = 0; i < comcount; i++)
				{
					Component* comp = complist[i];
					Isrc* current = dynamic_cast<Isrc*>(comp);
					if (current != NULL)
					{
						if (true)
						{
							if (current->get_node1() == arr[j] || current->get_node2() == arr[j])
							{
								complex <double> curr = current->getcomplex();
								I(j, 0) += curr;

							}
							if (current->get_node2() == arr[j-1] || current->get_node1() == arr[j - 1])
							{
								complex <double> curr = current->getcomplex();
								I(j, 0) += curr;
							}
						}
					}
				}
			}
			cout << "................" << endl;
			cout << m << endl;
			cout << "................" << endl;
			cout << I << endl;
			cout << "................" << endl;
			///////////////// calculate volts /////////////////////////////////
			MatrixXcd V = CalculateVoltNode(m, I); // function calculate volt of each node
			for (int i = 1; i < nonsimple_nodecount; i++)
			{
				nonsimple_Nodelist[i]->setvoltage(V(i - 1, 0));
			}
			for (int i = 0; i < branchcount; i++)
			{
				Branchlist[i]->Calculatecurrent();
			}
			cout << V;
			Save(nonsimple_Nodelist, Branchlist, nonsimple_nodecount, branchcount);
			return 0;
		}
		//////////////////code dependent ///////////////////////////////////////////////////
		for (int k = 0; k < num; k++)
		{
			complex <double> Admittance_s(0, 0);
			for (int i = 0; i < branchcount; i++)
			{
				if ((Branchlist[i]->getnode1()->getrank() == arr[k]) || (Branchlist[i]->getnode2()->getrank() == arr[k]))
				{
					complex <double> r;
					r = Branchlist[i]->get_Admittance();
					Admittance_s += r;
				}
			}
			m(k, k) = Admittance_s;
		}
		for (int v = 0; v < num; v++)
		{
			for (int k = v + 1; k < num; k++)
			{
				complex <double> Admittance_d(0, 0);
				for (int l = 0; l < branchcount; l++)
				{
					if (((Branchlist[l]->getnode1()->getrank() == arr[v]) || (Branchlist[l]->getnode2()->getrank() == arr[v]))
						&& ((Branchlist[l]->getnode1()->getrank() == arr[k]) || (Branchlist[l]->getnode2()->getrank() == arr[k])))
					{
						Admittance_d += Branchlist[l]->get_Admittance();
					}
				}
				m(v, k) = -Admittance_d;
				m(k, v) = -Admittance_d;
			}
		}
		for (int j = 0; j < num; j++)
		{
			for (int i = 0; i < comcount; i++)
			{
				Component* comp = complist[i];
				Isrc* current = dynamic_cast<Isrc*>(comp);
				if (current != NULL)
				{
					if (current->get_node1() == arr[j])
					{
						complex <double> curr = current->getcomplex();
						I(j, 0) += curr;

					}
					if (current->get_node2() == arr[j])
					{
						complex <double> curr = -current->getcomplex();
						I(j, 0) += curr;
					}
				}
			}
		}
		MatrixXcd V = CalculateVoltNode(m, I); // function calculate volt of each node
		for (int i = 1; i < nonsimple_nodecount; i++)
		{
			nonsimple_Nodelist[i]->setvoltage(V(i - 1, 0));
		}
		for (int i = 0; i < branchcount; i++)
		{
			Branchlist[i]->Calculatecurrent();
		}
		cout << "..........................." << endl;
		cout << "Matrix of admittance" << endl;
		cout << m << endl;
		cout << "..........................." << endl;
		cout << "Matrix of I" << endl;
		cout << I << endl;
		cout << "..........................." << endl;
		cout << "volt at nonsimplenode" << endl;
		cout << V << endl;
		///////////////////////////////////////////// calculate the value of voltage on simple node ///////////////////////////////////////////
		for (int i = 0; i < simple_nodecount; i++)
		{
			for (int j = 0; j < nonsimple_nodecount; j++)
			{
				if (simple_Nodelist[i]->getvoltage() != 0.0)
				{
					for (int k = 0; k < branchcount; k++)
					{
						if ((Branchlist[k]->getnode1() == simple_Nodelist[i] || Branchlist[k]->getnode2() == simple_Nodelist[i]) && (Branchlist[k]->getnode1() == nonsimple_Nodelist[j] || Branchlist[k]->getnode2() == nonsimple_Nodelist[j]))
						{
							complex<double>x = Branchlist[k]->getcurrent() * Branchlist[k]->getz();
							complex<double>c = 0.0;
							if (Branchlist[k]->getvolt() != NULL)
							{

								c = Branchlist[k]->getvolt()->getcomplex();
							}
							complex<double>e = nonsimple_Nodelist[j]->getvoltage();
							int N1 = Branchlist[k]->getnode1()->getrank();
							int N2 = Branchlist[k]->getnode2()->getrank();
							Component* comp2 = NULL;
							for (int i = 0; i < comcount; i++)
							{
								if (((complist[i]->get_node1() == N1) && (complist[i]->get_node2() == N2))
									|| ((complist[i]->get_node1() == N2) && (complist[i]->get_node2() == N1)))
								{
									comp2 = complist[i];
								}
							}
							int pos_node = comp2->get_node1();
							int neg_node = comp2->get_node2();
							if (pos_node == simple_Nodelist[i]->getrank() && neg_node == nonsimple_Nodelist[j]->getrank())
							{
								complex<double> cs = x + e + c;
								simple_Nodelist[i]->setvoltage(cs);
							}
							else if (neg_node == simple_Nodelist[i]->getrank() && pos_node == nonsimple_Nodelist[j]->getrank())
							{
								complex<double> cs = e - x - c;
								simple_Nodelist[i]->setvoltage(cs);
							}
							break;
						}
					}
				}
			}
		}
		int zero_volt = 0;
		int non_zerovolt = 0;
		for (int i = 0; i < simple_nodecount; i++)
		{
			if (simple_Nodelist[i]->getvoltage() == 0.0)
			{
				zero_volt++;
			}
			else
			{
				non_zerovolt++;
			}
		}
		if (non_zerovolt != 0)
		{
			Node** zero_voltlist = new Node * [zero_volt];
			Node** nonzero_voltlist = new Node * [non_zerovolt];
			int i1 = 0;
			int i2 = 0;
			for (int i = 0; i < simple_nodecount; i++)
			{
				if (simple_Nodelist[i]->getvoltage() == 0.0)
				{
					zero_voltlist[i1] = simple_Nodelist[i];
					i1++;
				}
				else
				{
					nonzero_voltlist[i2] = simple_Nodelist[i];
					i2++;
				}
			}

			for (int i = 0; i < i1; i++)
			{
				for (int j = 0; j < i2; j++)
				{
					if (zero_voltlist[i]->getvoltage() != 0.0)
					{
						for (int k = 0; k < branchcount; k++)
						{
							if ((Branchlist[k]->getnode1() == zero_voltlist[i] || Branchlist[k]->getnode2() == zero_voltlist[i]) && (Branchlist[k]->getnode1() == nonzero_voltlist[j] || Branchlist[k]->getnode2() == nonzero_voltlist[j]))
							{
								complex<double>x = Branchlist[k]->getcurrent() * Branchlist[k]->getz();
								complex<double>c = 0.0;
								if (Branchlist[k]->getvolt() != NULL)
								{
									c = Branchlist[k]->getvolt()->getcomplex();

								}
								complex<double>e = nonzero_voltlist[j]->getvoltage();
								int N1 = Branchlist[k]->getnode1()->getrank();
								int N2 = Branchlist[k]->getnode2()->getrank();
								Component* comp2 = NULL;
								for (int i = 0; i < comcount; i++)
								{
									if (((complist[i]->get_node1() == N1) && (complist[i]->get_node2() == N2))
										|| ((complist[i]->get_node1() == N2) && (complist[i]->get_node2() == N1)))
									{
										comp2 = complist[i];
									}
								}
								int pos_node = comp2->get_node1();
								int neg_node = comp2->get_node2();
								if (pos_node == zero_voltlist[i]->getrank() && neg_node == nonzero_voltlist[j]->getrank())
								{
									complex<double> cs = x + e + c;
									zero_voltlist[i]->setvoltage(cs);
								}
								else if (neg_node == zero_voltlist[i]->getrank() && pos_node == nonzero_voltlist[j]->getrank())
								{
									complex<double> cs = e - x - c;
									zero_voltlist[i]->setvoltage(cs);
								}
								break;
							}
						}
					}
				}
			}
		}
		cout << "\nvoltage at simple nodes" << endl;
		for (int i = 0; i < simple_nodecount; i++)
		{
			cout<<simple_Nodelist[i]<<" ";
		}
		Save(nonsimple_Nodelist, Branchlist, nonsimple_nodecount, branchcount);

		return 0;
	}
	return 0;
}
MatrixXcd CalculateVoltNode(MatrixXcd G, MatrixXcd I)
{
	MatrixXcd V;
	V = G.inverse() * I;
	return V;
}
int searcher(string x)
{
	int c;
	Component* comp = NULL;
	for (int i = 0; i < comcount; i++)
	{
		string name = complist[i]->get_name();
		if ((name.compare(x) == 0))
		{
			comp = complist[i];
			c = i;
		}
	}
	return c;
}
void Save(Node** list1, Branch** list2,int num1,int num2 )
{
	string name;
	cout << endl;
	cout << "enter the name of file (name .txt)" << endl;
	cin >> name;
	ofstream outputfile(name);
	outputfile << "the voltage at each node :" << endl;
	for (int i = 0; i < num1; i++)
	{
		outputfile << "v1" << " " << list1[i]->getvoltage() << endl;;
	}
	for (int i = 0; i < num2; i++)
	{
		int n1 =list2[i]->getnode1()->getrank();
		int n2 = list2[i]->getnode2()->getrank();
		outputfile << "I" << " " << n1 << " " << n2 << " " << list2[i]->getcurrent() << endl;
		
	}
	outputfile.close();
}
