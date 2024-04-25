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
	int N{};	// Number of unit cells along each axis.
	vector<Atom> atoms{};

	constexpr static double aParam{ 0.543 * 1e-9 };
	constexpr static double eps{ 1.60218e-19 * 2.17 };
	constexpr static double dt{ 0.5 * 1e-16 };
	constexpr static double m{ 4.66378961e-26 };
	constexpr static double dx{ aParam * 1e-5 };
	constexpr static double dy{ aParam * 1e-5 };
	constexpr static double dz{ aParam * 1e-5 };
	constexpr static double A{ 7.05 };
	constexpr static double B{ 0.602 };
	constexpr static double G{ 0.2095 * 1e-9 };
	constexpr static double r1{ 1.8 * G };
	constexpr static int p{ 4 };
	constexpr static double lambda{ 21.0 };
	constexpr static double gamma{ 1.20 };
	constexpr static double k{ 1.380649 * 1e-23 };
	double vMax = sqrt(3 * k * 300 / m);

	double xWall{};
	double yWall{};
	double zWall{};
	double t{};

	int n{ 0 };	// iters count
	int n_rlx{ 500 };	// iters until relaxation
	int n_temp{ 500 };	// iters until computing average temperature
	double alpha_rlx{ 0.8 };
	
	bool bRlx{ false };

	mutex* mtx{};
	vector<double> Ek_history;
public:
	void Relaxation();
	void Init(int size);
	void FindNeighbors();
	void CheckBoundaries();
	void RandomAtomsShift(double posShiftScale);
	void RandomAtomVelocity(double velShiftScale);
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