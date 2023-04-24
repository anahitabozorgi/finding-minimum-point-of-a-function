#include "exprtk.hpp"
#include <iostream>
#include <list>
#include <bitset>
#include <limits.h>
#include <cstring>
using namespace std;
using namespace exprtk;

bitset<32U> ToBinary(float f) {
	bitset<32U> binary = bitset<sizeof(float)* CHAR_BIT>(*reinterpret_cast<unsigned long*>(&f));
	return binary;
}
float ToFloat(bitset<32U> binary) {
	float yyy;
	//for (size_t i = 0; i < 32; i++)
	//{
	//	//binary[i] = 1;
	//}
	const unsigned long val = binary.to_ulong();
	memcpy(&yyy, &val, sizeof(float));
	return yyy;
}

class Node {
public:
	Node()
	{

	}
	Node(float x_) {
		x = x_;
		binary = ToBinary(x);
	}
	Node(bitset<32U> binary_) {
		x = ToFloat(binary_);
		binary = binary_;
	}
	float x;
	float y;
	bitset<32U> binary;
};

class Worker {
	symbol_table<float> symbol_table;
	expression<float> expression;
	parser<float> parser;
	float x;
	string xpr = "3x^2";
public:
	//float calc(float value) {
	//	x = value;
	//	return expression.value();
	//}
	void calc(Node& node) {
		x = node.x;
		node.y = expression.value();
	}
	Worker(string exp) {
		symbol_table.add_variable("x", x);
		symbol_table.add_constants();
		expression.register_symbol_table(symbol_table);
		parser.compile(exp, expression);
	}
};



bool compare_nocase(const Node& first, const Node& second)
{
	return first.y < second.y;
}
float random(float min, float max) {
	float r = (float)rand() / (RAND_MAX);
	return (r) * (max - min) + min;
}
float random01() {
	float r = (float)rand() / (RAND_MAX);
	return r;
}


//params
float min_ = INT_MAX / 2;
float max_ = INT_MIN / 2;
float best = 0;
int tries = 100;
int populationSize = 10;
float mutationAlpha = 0.2;
float mutationBeta = 0.5;




list<Node> BuildPopulation(Worker& w)
{
	list<Node> population;
	for (int i = 0; i < populationSize; i++)
	{
		Node t(random(min_, max_));
		w.calc(t);
		population.push_back(t);
	}
	return population;
}
void CrossOver(list<Node>& population, Worker& w)
{
	list<Node>::iterator it = population.begin();
	Node a1 = *it;
	it++;
	Node a2 = *it;

	int k2 = rand() % 30 + 2;
	int k1 = rand() % k2;

	bitset<32U> temp1;
	bitset<32U> temp2;
	for (int i = 0; i < k1; i++)
	{
		temp1[i] = a1.binary[i];
		temp2[i] = a2.binary[i];
	}
	for (int i = k1; i < k2; i++)
	{
		temp2[i] = a1.binary[i];
		temp1[i] = a2.binary[i];
	}
	for (int i = k2; i < 32; i++)
	{
		temp1[i] = a1.binary[i];
		temp2[i] = a2.binary[i];
	}
	Node a1a2(temp1);
	Node a1a3(temp2);
	w.calc(a1a2);
	w.calc(a1a3);
	population.push_back(a1a2);
	population.push_back(a1a3);
	population.sort(compare_nocase);
	population.pop_back();
	population.pop_back();
}
void Mutaion(list<Node>& population, Worker& w)
{
	bitset<32U>  a1b = population.front().binary;
	for (int i = 0; i < 32; i++)
	{
		if (random01() < mutationBeta)
		{
			a1b[i] = a1b[i] == 1 ? 0 : 1;
		}
	}
	Node a2(a1b);
	w.calc(a2);
	population.push_back(a2);
	population.sort(compare_nocase);
	population.pop_back();
}

int main() {
	srand(time(NULL));
	cout << "please enter f(x):\n";
	string h;
	cin >> h;
	Worker w(h);
	list<Node> population = BuildPopulation(w);
	population.sort(compare_nocase);


	int iter = 1;
	while (iter < tries)
	{
		iter++;
		CrossOver(population, w);
		if (random01() < mutationAlpha)
			Mutaion(population, w);
		cout << "\nBest Found So far...: x= " << population.front().x << " => F(x) =  " << population.front().y;
	}
}