// Includes some calculus tools.
// Romberg does numerical integration.
// Bisection calculates the x axis interceptipn point of a given function.

# ifndef CALCULUS_H
# define CALCULUS_H
# include <iostream>
# include <vector>
# include <asm-generic/errno.h>
# include <math.h>
# include <fstream>
# include <iomanip>
# ifndef TOLERANCE
	# define TOLERANCE 1e-10
# endif
# define MAX 50


using namespace std;
vector<double> bisection (double xh, double xl, double (*f)(double));
vector <vector<double> > sandwich(double h, double l, double (*f)(double), int N=10000);


vector<double> bisection (double xh, double xl, double (*f)(double)) {
	int STOP = 50;
	int iterCount;
	vector <vector <double> > brackets = sandwich (xh, xl, f);
	vector <double> solutions;
	
	ofstream out;
	out.open("bisection.log");
	out << "xh: " << xh << "\t\txl: " << xl << endl;
	out << "Tolerance: " << TOLERANCE << endl;
	out << endl;
	out.precision(15);
	out << "Root\tCycle\t\tx\t\tf(x)" << endl;
	out << "--------------------------------------------------" << endl;
	
	for (vector <vector<double> >::iterator i= brackets.begin(); i != brackets.end(); i++) {
		vector <double> x (*i);
		for (iterCount=0; iterCount<STOP; iterCount++) {
			double xmiddle = (x.at(0)+x.at(1))/2;
			double fmiddle = f(xmiddle);
			
			out << i - brackets.begin() + 1 << "\t" << iterCount+1 << "\t\t" << setiosflags(ios::showpoint) << xmiddle << "\t\t" << fmiddle << endl;
			
			if (fmiddle == 0 || (x.at(1) - x.at(0)) < TOLERANCE) {
				solutions.push_back(xmiddle);
				break;
			}
		
			double f1 = f (x.at(1));
			double f0 = f (x.at(0));
			
			if (fmiddle*f0 < 0)
				x.at(1) = xmiddle;
			else
				x.at(0) = xmiddle;
		}
		
		if (iterCount == STOP) {
			cout << "\nERROR: Too many iterations." << endl;
			out.close();
			throw(E2BIG);
		}
	}
	
	out.close();
	return solutions;
}
		
	
vector <vector<double> > sandwich(double h, double l, double (*g)(double), int N) {
	double dx = (h - l)/N;
	double x[N];
	double f[N];
	
	for (int i=0; i<N; i++) {
		x[i] = l + dx*i;
		f[i] = g(x[i]);
	}
	
	vector<vector <double> > intervals;
	for (int i=1; i<(N-1); i++) {
		if (f[i]*f[i-1] < 0) {
			vector <double> bracket(2);
			bracket.at(0) = x[i-1];
			bracket.at(1) = x[i];
			intervals.push_back(bracket);
		}
	}
	
	if (intervals.size() == 0) {
		cout << "\nERROR: The function could not be trapped within the given boundaries." << endl;
		throw (EAGAIN);
	}
	
	return intervals;
}


double romberg (double a, double b, double (*f)(double)) {
	if (a == b) {
		cout << "\nERROR: integration domain is a point." << endl;
		throw(EINVAL);
	}


	if (a > b) {
		return -romberg(b,a,f);
	}
	
		
	double quads [MAX][MAX];
	double h = b-a;
	quads[0][0] = 0.5*h*(f(a)+f(b));
		
	ofstream out;
	out.open("romberg.log");
	out << "#Cycle\t\t\tf(x)dx" << endl;
	out << "--------------------------------------------------" << endl;
	out << scientific;
	
	for (int i=1; i<MAX; i++) {
		double sum=0;
		double x = a+h/2;
		while (x < b) {
			sum+= f(x);
			x+=h;
		}

		quads[i][0] = 0.5*quads[i-1][0] + 0.5*h*sum;

		h/=2;
		for (int j=1; j<=i; j++)
			quads[i][j] = (pow(4,j)*quads[i][j-1] - quads[i-1][j-1]) /(pow(4,j)-1);
		
		out << i << "\t\t\t" << quads[i][i] << endl;
		if (isnan(quads[i][0])) {
			cout << "\nERROR: Function evaluation returned non-real value." << endl;
			throw(EDOM);
		}
		
		if (fabs(quads[i][i]-quads[i-1][i-1]) < TOLERANCE && i>5) {
			out.close();
			return quads[i][i];
		}
	}
	
	cout << "\nERROR: Romberg did not converge after " << MAX << " iterations." << endl;
	out.close();
	throw(E2BIG);
}

#endif
