#pragma once
#include <iostream>
using namespace std;
class Node;
class Component
{
private:
	Node* node1;
	Node* node2;
	string name;
public:
	Component(Node* x1, Node* x2, string n);
	void set_Node1(Node* x);
	void set_Node2(Node* x);
	void set_Name(string x);
	Node* get_Node1();
	Node* get_Node2();
	string get_Name();
};

