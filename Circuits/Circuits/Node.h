#pragma once
#include <iostream>
#include <complex>       
using namespace std;
class Node {
private:
	int rank;    // number of node 
	bool simple;
	int num_connections;  // number of nodes that connected to this node 
	complex <double> voltage;
	Node* connections[10];
public:
	Node(int);
	//return true if simple &&  false if non_simple
	bool ifsimple();
	void increment_connections(Node* Node2);
	void setvoltage(complex <double>);
	complex <double> getvoltage();
	int getrank();
	Node** getNodeconnections();
};