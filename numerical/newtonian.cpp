# include <iostream>
# include <vector>
# include <iomanip>
# include <fstream>
# include <math.h>
# include <stdlib.h>
# include "ludcmp.h"
# include <asm-generic/errno.h>

using namespace std;
void function (vector<double> &x, vector<double> &fvec, vector<vector <double> > &fjac);
void mnewt(const int ntrial, vector <double> &x, const double tolx, const double tolf);


int main (void) {
	vector <double> x(2);
	x.at(0) = 9;
	x.at(1) = -1;
	
	cout.precision(15);
	cout << x.at(0) << "\t" << x.at(1) << endl;
	mnewt(50, x, 1e-10, 1e-10);
	cout << x.at(0) << "\t" << x.at(1) << endl;
	
	return 0;
}



void function (vector<double> &x, vector<double> &fvec, vector<vector <double> > &fjac) {
	fjac.at(0).at(0) = 2*x.at(0);
	fjac.at(0).at(1) = 2*x.at(1);
	fjac.at(1).at(0) = 1;
	fjac.at(1).at(1) = -1;
	
	fvec.at(0) = x.at(0)*x.at(0) + x.at(1)*x.at(1) -1;
	fvec.at(1) = x.at(0) - x.at(1);
		
	return;
}


void mnewt(const int ntrial, vector <double> &x, const double tolx, const double tolf) {
	int n = x.size();
	vector <double> p(n),fvec(n);
	
	vector<vector <double> > fjac (n);
	for (int i=0; i<n; i++)
		fjac.at(i).resize(n);


	ofstream out;
	out.open("multiNewton.log");
	out << "\tTolerance x: " << tolx << "\tTolerance x: " << tolf << endl;
	out << "\txGuessed: " << x.at(0) << "\t\tyGuessed: " << x.at(1) << endl;
	out << endl;
	out.precision(15);
	out << "Cycle\t\tx\t\t\t\ty\t\t\t\tf1(x)\t\t\t\tf2(x)" << endl;
	out << "------------------------------------------------------------------------------------------------------------------------------------" << endl;

	for (int k=0; k<ntrial; k++) {
		function(x,fvec,fjac);
		out << k+1 << "\t\t" << setiosflags(ios::showpoint) << x.at(0) << "\t\t" << x.at(1) << "\t\t" << fvec.at(0) << "\t\t" << fvec.at(1) << endl;
		
		double errf = 0;
		for (int i=0;i<n;i++)
			errf += fabs(fvec[i]);

		//Check function convergence.
		if (errf <= tolf) {
			out.close();
			return;
		}
		
		for (int i=0;i<n;i++)
			p[i] = -fvec[i];

		//Right-hand side of linear equations.
		LUdcmp alu(fjac);

		//Solve linear equations using LU decomposition.
		alu.solve(p,p);

		double errx = 0;
		for (int i=0; i<n; i++) {
			errx += fabs(p[i]);
			x[i] += p[i];
		}
		
		if (errx <= tolx) {
			out.close();
			return;
		}
	}
	
	cout << "ERROR: Too many iterations." << endl;
	out.close();
	throw(E2BIG);
}
