#pragma once
#include "Atom.h"
#include <vector>
#include <thread>
#include <mutex>
using namespace std;

struct Vec3 {
	double x;
	double y;
	double z;
	double Length() { return sqrt(x * x + y * y + z * z); };
};


class Silicon
{
public:
	int N;	// Number of unit cells in one axis.
	vector<Atom> atoms;
	double aParam = 0.543 * 1e-9;
	//double eps = 1.60218e-19 * 2.17;
	double eps = 1.60218e-19 * 2.17;
	double xWall;
	double yWall;
	double zWall;
	double t;
	double dt = 0.5 * 1e-16;
	double m = 4.66378961e-26;
	//double m = 4.6457e-26;
	double dx = aParam * 1e-5;
	double dy = aParam * 1e-5;
	double dz = aParam * 1e-5;

	double A = 7.05;
	double B = 0.602;
	double G = 0.2095 * 1e-9;
	double r1 = 1.8 * G;
	int p = 4;
	double lambda = 21.0;
	double gamma = 1.20;
	double k = 1.380649 * 1e-23;
	double vMax = sqrt(3 * k * 300 / m);

	int n_rlx = 500;
	int n_temp = 500;
	double alpha_rlx = 0.8;
	int n = 0;
	bool bRlx = false;

	mutex* mtx;
	vector<double> Ek_history;
public:
	void Relaxation();
	void Init(int n);
	void FindNeighbors();
	void CheckBoundaries();
	void RandomAtomsShift(bool pos, bool vel);
	void Verlet(bool restart = false);
	double Force(Atom& atom, double dx, double dy, double dz);
	double f2(Vec3& rij);
	double f3(Vec3& rij, Vec3& rik);
	double GetTotalEnergy(bool toFile = false);
	void Reset() { Init(N); }
	void DeleteCentralAtom();
	void AvgTemperature();
};


int Sign(double &val);