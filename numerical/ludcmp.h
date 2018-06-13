// Performs LU decomposition on a provided matrix

# include <vector>
# include <iostream>
# include <math.h>
# include <stdlib.h>

using namespace std;


struct LUdcmp {
	int n;
	vector<vector <double> > lu;
	vector <int> indx;
	double d;
	LUdcmp(vector<vector <double> > &a); // Constructor. Argument is the matrix A.
	void solve(vector<double> &b, vector<double> &x); // Solve for a single right-hand side.
	void solve(vector<vector <double> > &b, vector<vector <double> > &x); // Solve for multiple right-hand sides.
	void inverse(vector<vector <double> > &ainv); // Calculate matrix inverse A 1 .
	double det(); // Return determinant of A.
	void mprove(vector<double> &b, vector<double> &x);
	vector<vector <double> > &aref;
};



LUdcmp::LUdcmp(vector<vector <double> > &a) 
	: n(a.at(0).size()), lu(a), aref(a), indx(n) {
	
	const double TINY=1.0e-40; // A small number.
	int imax;
	double big,temp;
	vector<double> vv(n); // vv stores the implicit scaling of each row.
	d=1.0;

	for (int i=0;i<n;i++) {
		big=0.0;
		for (int j=0;j<n;j++)
			if ((temp=abs(lu[i][j])) > big) big=temp;
			
		if (big == 0.0) throw("Singular matrix in LUdcmp");

		vv.at(i) = 1.0/big;
	}
	
	for (int k=0;k<n;k++) {
		big=0.0;
		for (int i=k;i<n;i++) {
			temp=vv[i]*abs(lu[i][k]);
			if (temp > big) {
				big=temp;
				imax=i;
			}
		}
		if (k != imax) {
			for (int j=0;j<n;j++) {
				temp=lu[imax][j];
				lu[imax][j]=lu[k][j];
				lu[k][j]=temp;
			}
			d = -d;

			vv[imax]=vv[k];
		}
		indx[k]=imax;
		
		if (lu[k][k] == 0.0) lu[k][k]=TINY;

		for (int i=k+1;i<n;i++) {
			temp=lu[i][k] /= lu[k][k]; // Divide by the pivot element.
			for (int j=k+1;j<n;j++) // Innermost loop: reduce remaining submatrix.
				lu[i][j] -= temp*lu[k][j];
		}
	}
}


void LUdcmp::solve(vector<double> &b, vector<double> &x) {
	int ii=0,ip;
	double sum;
	
	if (b.size() != n || x.size() != n)
		throw("LUdcmp::solve bad sizes");
	
	for (int i=0;i<n;i++)
		x[i] = b[i];
		
	for (int i=0;i<n;i++) {
		ip=indx[i];
		sum=x[ip];
		x[ip]=x[i];
		if (ii != 0)
			for (int j=ii-1;j<i;j++)
				sum -= lu[i][j]*x[j];
		else if (sum != 0.0)
			ii=i+1;
		x[i]=sum;
	}
	
	for (int i=n-1;i>=0;i--) {
		sum=x.at(i);
		for (int j=i+1;j<n;j++)
			sum -= lu[i][j]*x[j];
			x[i]=sum/lu[i][i];
	}
}


void LUdcmp::solve(vector<vector <double> > &b, vector<vector <double> > &x) {
	int m=b.size();
	if (b.at(0).size()!= n || x.at(0).size() != n || b.size() != x.size())
		throw("LUdcmp::solve bad sizes");
		
	vector <double> xx(n);
	
	for (int j=0; j<m; j++) {
		for (int i=0; i<n; i++)
			xx[i] = b[i][j];
		solve(xx,xx);
		for (int i=0; i<n; i++)
			x[i][j] = xx[i];
	}
}


void LUdcmp::inverse(vector<vector <double> > &ainv) {
	ainv.resize(n);
	for (int i=0; i<n; i++)
		ainv.at(i).resize(n);

	for (int i=0;i<n;i++) {
		for (int j=0;j<n;j++) ainv[i][j] = 0.0;
		ainv[i][i] = 1.0;
	}
	
	solve(ainv,ainv);
}


double LUdcmp::det() {
	double dd = d;
	for (int i=0; i<n; i++)
		dd *= lu[i][i];
	return dd;
}
