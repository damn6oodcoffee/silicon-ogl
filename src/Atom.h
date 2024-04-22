#pragma once
#include <vector>
using namespace std;

class Atom
{
public:
	double x, y, z;
	double vx, vy, vz;
	double rad = 1.0 * 1.8 * 0.2095 * 1e-9;
	vector<Atom*> neighbours;

};

