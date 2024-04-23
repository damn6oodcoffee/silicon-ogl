#include "Silicon.h"

void Silicon::Init(int size) {
	N = size;
	this->n = 0;
	//aParam = 3.0 / N;
	atoms.clear();
	
	xWall = yWall = zWall = aParam * N;

	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			for (int k = 0; k < N; k++) {
				vector<Atom> cellAtoms(8);
				
				
				cellAtoms[0].x = aParam * i;
				cellAtoms[0].y = aParam * j;
				cellAtoms[0].z = aParam * k;
				

				cellAtoms[1].x = aParam * i + 0.5*aParam;
				cellAtoms[1].y = aParam * j + 0.5*aParam;
				cellAtoms[1].z = aParam * k;

				cellAtoms[2].x = aParam * i + 0.5*aParam;
				cellAtoms[2].y = aParam * j;
				cellAtoms[2].z = aParam * k + 0.5*aParam;

				cellAtoms[3].x = aParam * i;
				cellAtoms[3].y = aParam * j + 0.5*aParam;
				cellAtoms[3].z = aParam * k + 0.5*aParam;

				cellAtoms[4].x = aParam * i + 0.25*aParam;
				cellAtoms[4].y = aParam * j + 0.25*aParam;
				cellAtoms[4].z = aParam * k + 0.25*aParam;

				cellAtoms[5].x = aParam * i + 0.75*aParam;
				cellAtoms[5].y = aParam * j + 0.75*aParam;
				cellAtoms[5].z = aParam * k + 0.25*aParam;

				cellAtoms[6].x = aParam * i + 0.75*aParam;
				cellAtoms[6].y = aParam * j + 0.25*aParam;
				cellAtoms[6].z = aParam * k + 0.75*aParam;

				cellAtoms[7].x = aParam * i + 0.25*aParam;
				cellAtoms[7].y = aParam * j + 0.75*aParam;
				cellAtoms[7].z = aParam * k + 0.75*aParam;

				for (auto& el : cellAtoms) {
					el.vx = el.vy = el.vz = 0;
				}

				atoms.insert(atoms.end(), cellAtoms.begin(), cellAtoms.end());

			}
		}
	}
	FindNeighbors();

}

void Silicon::FindNeighbors() {

	double rad = 1.25 * aParam * sqrt(3) / 4;
	//rad = atoms[0].rad / 1.5;
	rad = atoms[0].rad;
	Vec3 rij;
	for (auto& atom : atoms) {
		atom.neighbours.clear();
		for (auto& other : atoms) {
			if (&atom == &other) {
				continue;
			}

			rij.x = other.x - atom.x;
			if (abs(rij.x) > xWall / 2) {
				rij.x -= Sign(rij.x) * xWall;
			}
			rij.y = other.y - atom.y;
			if (abs(rij.y) > yWall / 2) {
				rij.y -= Sign(rij.y) * yWall;
			}
			rij.z = other.z - atom.z;
			if (abs(rij.z) > zWall / 2) {
				rij.z -= Sign(rij.z) * zWall;
			}
			
			if (rij.Length() < rad) {
				atom.neighbours.push_back(&other);
			}

		}
	}
	return;

}

void Silicon::CheckBoundaries() {
	for (auto& atom : atoms) {
		while (atom.x < 0 || atom.x > xWall) {
			if (atom.x < 0) atom.x += xWall;
			else if (atom.x > xWall) atom.x -= xWall;
		}

		while (atom.y < 0 || atom.y > yWall) {
			if (atom.y < 0) atom.y += yWall;
			else if (atom.y > yWall) atom.y -= yWall;
		}

		while (atom.z < 0 || atom.z > zWall) {
			if (atom.z < 0) atom.z += zWall;
			else if (atom.z > zWall) atom.z -= zWall;
		}
	}
}


double Silicon::Force(Atom& atom, double dx, double dy, double dz) {
	
	Vec3 rij;
	Vec3 rik;
	Vec3 rij_d;
	Vec3 rik_d;
	double nextU = 0;
	double prevU = 0;
	for (int j = 0; j < atom.neighbours.size(); j++) {

		rij.x = atom.neighbours[j]->x - atom.x;
		if (abs(rij.x) > xWall/2) {
			rij.x -= Sign(rij.x) * xWall;
		}
		rij.y = atom.neighbours[j]->y - atom.y;
		if (abs(rij.y) > yWall/2) {
			rij.y -= Sign(rij.y) * yWall;
		}
		rij.z = atom.neighbours[j]->z - atom.z;
		if (abs(rij.z) > zWall/2) {
			rij.z -= Sign(rij.z) * zWall;
		}

		rij_d = rij;

		rij_d.x -= dx;
		rij_d.y -= dy;
		rij_d.z -= dz;

		nextU += eps * f2(rij_d);

		rij_d.x += 2 * dx;
		rij_d.y += 2 * dy;
		rij_d.z += 2 * dz;

		prevU += eps * f2(rij_d);


		for (int k = j + 1; k < atom.neighbours.size(); k++) {
		
			rik.x = atom.neighbours[k]->x - atom.x;
			if (abs(rik.x) > xWall/2) {
				rik.x -= Sign(rik.x) * xWall;
			}
			rik.y = atom.neighbours[k]->y - atom.y;
			if (abs(rik.y) > yWall/2) {
				rik.y -= Sign(rik.y) * yWall;
			}
			rik.z = atom.neighbours[k]->z - atom.z;
			if (abs(rik.z) > zWall/2) {
				rik.z -= Sign(rik.z) * zWall;
			}

			rij_d = rij;
			rik_d = rik;

			rij_d.x -= dx;
			rij_d.y -= dy;
			rij_d.z -= dz;

			rik_d.x -= dx;
			rik_d.y -= dy;
			rik_d.z -= dz;

			nextU += eps * f3(rij_d, rik_d);

			rij_d.x += 2 * dx;
			rij_d.y += 2 * dy;
			rij_d.z += 2 * dz;
					
			rik_d.x += 2 * dx;
			rik_d.y += 2 * dy;
			rik_d.z += 2 * dz;

			prevU += eps * f3(rij_d, rik_d);
		}
	}

	return (prevU - nextU) / (2 * (dx + dy + dz));

}

void Silicon::Verlet(bool restart) {
	while (!mtx->try_lock()) {};
	CheckBoundaries();
	vector<double> forceX;
	vector<double> forceY;
	vector<double> forceZ;
	forceX.resize(atoms.size());
	forceY.resize(atoms.size());
	forceZ.resize(atoms.size());


	for (int i = 0; i < atoms.size(); i++) {
		forceX[i] = Force(atoms[i], dx, 0, 0);
		forceY[i] = Force(atoms[i], 0, dy, 0);
		forceZ[i] = Force(atoms[i], 0, 0, dz);
		atoms[i].x = atoms[i].x + atoms[i].vx * dt + 0.5 * forceX[i] * dt * dt / m;
		atoms[i].y = atoms[i].y + atoms[i].vy * dt + 0.5 * forceY[i] * dt * dt / m;
		atoms[i].z = atoms[i].z + atoms[i].vz * dt + 0.5 * forceZ[i] * dt * dt / m;
	}
	CheckBoundaries();
	FindNeighbors();

	for (int i = 0; i < atoms.size(); i++) {
		atoms[i].vx = atoms[i].vx + 0.5 * (forceX[i] + Force(atoms[i], dx, 0, 0)) * dt / m;
		atoms[i].vy = atoms[i].vy + 0.5 * (forceY[i] + Force(atoms[i], 0, dy, 0)) * dt / m;
		atoms[i].vz = atoms[i].vz + 0.5 * (forceZ[i] + Force(atoms[i], 0, 0, dz)) * dt / m;
	}

	if (bRlx && n % n_rlx == 0) Relaxation();
	if (n % n_temp == 0) AvgTemperature();
	GetTotalEnergy(true);
	n++;
	mtx->unlock();
}

double Silicon::f2(Vec3& rij) {
	
	double rij_abs = rij.Length();
	if (rij_abs >= r1) {
		return 0;
	}
	return A * (B * pow(G / rij_abs, p) - 1) * exp(G / (rij_abs - r1));
}

double Silicon::f3(Vec3& rij, Vec3& rik) {
	
	double rij_abs = rij.Length();
	double rik_abs = rik.Length();

	double cosPhi = (rij.x * rik.x + rij.y * rik.y + rij.z * rik.z) / (rij_abs * rik_abs);

	if (rij_abs >= r1 || rik_abs >= r1) {
		return 0;
	}
	return lambda * exp(gamma * G / (rij_abs - r1) + gamma * G / (rik_abs - r1)) * (cosPhi + 1.0 / 3) * (cosPhi + 1.0 / 3);

}

void Silicon::RandomAtomsShift() {
	
	for (auto& atom : atoms) {

		atom.x += 0.02 * aParam * (2.0 * static_cast<double>(rand()) / RAND_MAX - 1.0);
		atom.y += 0.02 * aParam * (2.0 * static_cast<double>(rand()) / RAND_MAX - 1.0);
		atom.z += 0.02 * aParam * (2.0 * static_cast<double>(rand()) / RAND_MAX - 1.0);
		
	}
	CheckBoundaries();
}

void Silicon::RandomAtomVelocity() {

	for (auto& atom : atoms) {

		atom.vx = (2.0 * static_cast<double>(rand()) / RAND_MAX - 1.0);
		atom.vy = (2.0 * static_cast<double>(rand()) / RAND_MAX - 1.0);
		atom.vz = (2.0 * static_cast<double>(rand()) / RAND_MAX - 1.0);

		double v_abs = sqrt(atom.vx * atom.vx + atom.vy * atom.vy + atom.vz * atom.vz);

		atom.vx *= vMax / v_abs;
		atom.vy *= vMax / v_abs;
		atom.vz *= vMax / v_abs;
		
	}
	CheckBoundaries();
}

double Silicon::GetTotalEnergy(bool toFile) {


	if (!toFile) {
	
		double Ek = 0;
		for (auto& atom : atoms) {
			Ek += m * (atom.vx * atom.vx + atom.vy * atom.vy + atom.vz * atom.vz) / 2;
		}

		double Ep = 0;
		Vec3 rij;
		Vec3 rik;
		for (auto& atom : atoms) {

			for (int j = 0; j < atom.neighbours.size(); j++) {

				rij.x = atom.neighbours[j]->x - atom.x;
				if (abs(rij.x) > xWall / 2) {
					rij.x -= Sign(rij.x) * xWall;
				}
				rij.y = atom.neighbours[j]->y - atom.y;
				if (abs(rij.y) > yWall / 2) {
					rij.y -= Sign(rij.y) * yWall;
				}
				rij.z = atom.neighbours[j]->z - atom.z;
				if (abs(rij.z) > zWall / 2) {
					rij.z -= Sign(rij.z) * zWall;
				}

				Ep += eps * f2(rij) / 2;


				for (int k = j+1; k < atom.neighbours.size(); k++) {

					rik.x = atom.neighbours[k]->x - atom.x;
					if (abs(rik.x) > xWall / 2) {
						rik.x -= Sign(rik.x) * xWall;
					}
					rik.y = atom.neighbours[k]->y - atom.y;
					if (abs(rik.y) > yWall / 2) {
						rik.y -= Sign(rik.y) * yWall;
					}
					rik.z = atom.neighbours[k]->z - atom.z;
					if (abs(rik.z) > zWall / 2) {
						rik.z -= Sign(rik.z) * zWall;
					}

					Ep += eps * f3(rij, rik);


				}
			}
		}

		return 0;
	}

	/*
	ofstream myfile("energy.txt", ios::app);
	if (myfile.is_open())
	{
	
		double Ek = 0;
		for (auto& atom : atoms) {
			Ek += m*(atom.vx * atom.vx + atom.vy * atom.vy + atom.vz * atom.vz)/2;
		}

		Ek_history.push_back(Ek);

		double Ep = 0;
		Vec3 rij;
		Vec3 rik;
		for (auto& atom : atoms) {
			for (int j = 0; j < atom.neighbours.size(); j++) {

				rij.x = atom.neighbours[j]->x - atom.x;
				if (abs(rij.x) > xWall / 2) {
					rij.x -= Sign(rij.x) * xWall;
				}
				rij.y = atom.neighbours[j]->y - atom.y;
				if (abs(rij.y) > yWall / 2) {
					rij.y -= Sign(rij.y) * yWall;
				}
				rij.z = atom.neighbours[j]->z - atom.z;
				if (abs(rij.z) > zWall / 2) {
					rij.z -= Sign(rij.z) * zWall;
				}

				Ep += eps * f2(rij) / 2;


				for (int k = j + 1; k < atom.neighbours.size(); k++) {

					rik.x = atom.neighbours[k]->x - atom.x;
					if (abs(rik.x) > xWall / 2) {
						rik.x -= Sign(rik.x) * xWall;
					}
					rik.y = atom.neighbours[k]->y - atom.y;
					if (abs(rik.y) > yWall / 2) {
						rik.y -= Sign(rik.y) * yWall;
					}
					rik.z = atom.neighbours[k]->z - atom.z;
					if (abs(rik.z) > zWall / 2) {
						rik.z -= Sign(rik.z) * zWall;
					}

					Ep += eps * f3(rij, rik);


				}
			}
		}

		myfile << Ek / 1.60218e-19 << '\t' << Ep / 1.60218e-19 << '\t' << (Ep + Ek) / 1.60218e-19 << '\n';
		myfile.close();


		ofstream fake("energy_fake.txt", ios::app);
		fake << (Ep + Ek) / 1.60218e-19;
		fake.close();
	}
	 */
	return 0;
};

int Sign(double &val){
	if (val == 0) return 0;
	else return val > 0 ? 1 : -1;
}

void Silicon::DeleteCentralAtom() {
	Atom* nearestToCenter = &atoms[0];
	double xCenter = xWall / 2;
	double yCenter = yWall / 2;
	double zCenter = zWall / 2;

	for (auto& atom : atoms) {
		if (pow(nearestToCenter->x - xCenter, 2) + pow(nearestToCenter->y - yCenter, 2) + pow(nearestToCenter->z - zCenter, 2) > pow(atom.x - xCenter, 2) + pow(atom.y - yCenter, 2) + pow(atom.z - zCenter, 2)) {
			nearestToCenter = &atom;
		}
	}

	for (int i = 0; i < atoms.size(); i++) {
		if (&atoms[i] == nearestToCenter) {
			atoms.erase(atoms.begin() + i);
			break;
		}
	}

	FindNeighbors();
}

void Silicon::Relaxation() {
	for (auto& atom : atoms) {
		atom.vx *= alpha_rlx;
		atom.vy *= alpha_rlx;
		atom.vz *= alpha_rlx;
	}
}

void Silicon::AvgTemperature() {
	int atoms_count = atoms.size();
	int time_pts = Ek_history.size();
	double avg_Ek = 0;
	for (double val : Ek_history) {
		avg_Ek += val;
	}
	avg_Ek = avg_Ek / time_pts / atoms_count;
	double avg_T = avg_Ek * 2 / (3 * k);
	//ofstream myfile("temp.txt", ios::app);
	//myfile << avg_T << '\n';
	//myfile.close();
	Ek_history.clear();


}