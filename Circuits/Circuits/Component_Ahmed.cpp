#include "Component.h"
Component::Component(Node* x1, Node* x2, string n)
{
	name = n;
	node1 = x1;
	node2 = x2;
}
void Component::set_Node1(Node* x)
{
	node1 = x;
}
void Component::set_Node2(Node* x)
{
	node2 = x;
}
void Component::set_Name(string x)
{
	name = x;
}
Node* Component::get_Node1()
{
	return node1;
}
Node* Component::get_Node2()
{
	return node2;
}
string Component::get_Name()
{
	return name;
}
