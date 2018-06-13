# include <stdio.h>
# include <iostream>
# include <vector>
# include <asm-generic/errno.h>
# include <math.h>
# include <fstream>
# include <iomanip>

using namespace std;

vector<double> bisection (double xh, double xl, int noPoints, double tolerance);
vector <vector<double> > sandwich(double h, double l, int N);
double newton (double guess, double tolerance);
vector<double> falseposition (double xh, double xl, int noPoints, double tolerance);
double function (double x);
double dfunction (double x);

double quadPerturbation (double a, double b, double c, double tolerance);
double quadDiscriminant (double a, double b, double c);
double quadDiscriminant2 (double a, double b, double c);
void print (vector <double> &x, int precision = 15);
double quadEvaluate (double a, double b, double c, double x);


int main(void) {
	double tolerance = 1e-10;
	cout.precision(20);
		
	vector <double> x = bisection (10, -10, 1000, tolerance);
	cout << "Bisection\t";
	print(x);
	
	double y = newton(0, tolerance);
	cout << "Newton:\t" << y << "\t" << function(y) << endl;
	
	vector <double> z = falseposition (10, -10, 1000, tolerance);
	cout << "FalsePos\t";
	print(z);
	
	y = quadDiscriminant(1, -10000, 1);
	cout << "Discriminant:\t" << y << "\t" << quadEvaluate(1,-1e4, 1, y) << endl;
	
	y = quadDiscriminant2(1, -10000, 1);
	cout << "Discriminant2:\t" << y << "\t" << quadEvaluate(1,-1e4, 1, y) << endl;
	
	y = quadPerturbation(1, -10000, 1, tolerance);
	cout << "Perturbation:\t" << y << "\t" << quadEvaluate(1,-1e4, 1, y) << endl;
	
	return 0;
}


double quadEvaluate (double a, double b, double c, double x) {
	return (a*x*x + b*x + c);
}


void print (vector <double> &x, int precision) {
	for (int i=0; i< x.size(); i++) {
		cout << i+1 << "\t";
		cout.precision(precision);
		cout << x.at(i) << "\t" << function(x.at(i)) << endl;
	}
	
	return;
}


vector<double> bisection (double xh, double xl, int noPoints, double tolerance) {
	int STOP = 50;
	int iterCount;
	vector <vector <double> > brackets = sandwich (xh, xl, noPoints);
	vector <double> solutions;
	
	ofstream out;
	out.open("bisection.log");
	out << "xh: " << xh << "\t\txl: " << xl << endl;
	out << "#Points: " << noPoints << "\tTolerance: " << tolerance << endl;
	out << endl;
	out.precision(15);
	out << "Root\tCycle\t\tx\t\tf(x)" << endl;
	out << "--------------------------------------------------" << endl;
	
	for (vector <vector<double> >::iterator i= brackets.begin(); i != brackets.end(); i++) {
		vector <double> x (*i);
		for (iterCount=0; iterCount<STOP; iterCount++) {
			double xmiddle = (x.at(0)+x.at(1))/2;
			double fmiddle = function(xmiddle);
			
			out << i - brackets.begin() + 1 << "\t" << iterCount+1 << "\t\t" << setiosflags(ios::showpoint) << xmiddle << "\t\t" << fmiddle << endl;
			
			if (fmiddle == 0 || (x.at(1) - x.at(0)) < tolerance) {
				solutions.push_back(xmiddle);
				break;
			}
		
			double f1 = function (x.at(1));
			double f0 = function (x.at(0));
			
			if (fmiddle*f0 < 0)
				x.at(1) = xmiddle;
			else
				x.at(0) = xmiddle;
		}
		
		if (iterCount == STOP) {
			cout << "ERROR: Too many iterations." << endl;
			out.close();
			throw(E2BIG);
		}
	}
	
	out.close();
	return solutions;
}
		
	
vector <vector<double> > sandwich(double h, double l, int N=100) {
	double dx = (h - l)/N;
	double x[N];
	double f[N];
	
	for (int i=0; i<N; i++) {
		x[i] = l + dx*i;
		f[i] = function(x[i]);
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
		cout << "ERROR: The function could not be trapped within the given boundaries." << endl;
		throw (EAGAIN);
	}
	
	return intervals;
}


double newton (double guess, double tolerance) {
	int STOP = 50;
	double x=guess;
	double dx;
	
	ofstream out;
	out.open("newton.log");
	out << "Guess: " << guess << "\tTolerance: " << tolerance << endl;
	out << endl;
	out << "Cycle\t\tx\t\t\tf(x)" << endl;
	out << "------------------------------------------------------------" << endl;
	out.precision(15);
	
	for (int i=0; i<STOP; i++) {
		double f = function(x);
		double df = dfunction(x);
		
		dx = f/df;
		x -= dx;
		dx>=0 ? true : dx = -dx;
		
		out << i+1 << "\t\t" << setiosflags(ios::showpoint) << x << "\t\t" << function(x) << endl;
		
		if (f == 0 || dx < tolerance) {
			out.close();
			return x;
		}
	}
	
	cout << "ERROR: Too many iterations." << endl;
	out.close();
	throw(E2BIG);
}


vector <double> falseposition (double xh, double xl, int noPoints, double tolerance) {
	int STOP = 500;
	vector <vector <double> > brackets = sandwich (xh, xl, noPoints);
	vector <double> solutions (0);
	
	ofstream out;
	out.open("falseposition.log");
	out << "xh: " << xh << "\t\txl: " << xl << endl;
	out << "#Points: " << noPoints << "\tTolerance: " << tolerance << endl;
	out << endl;
	out.precision(15);
	out.width(15);
	out << "Root\tCycle\t\tx\t\tf(x)" << endl;
	out << "--------------------------------------------------" << endl;
	
	for (vector<vector<double> >::iterator i= brackets.begin(); i != brackets.end(); i++) {
		vector<double> x(*i);
		int iterCount;
		for (iterCount=0; iterCount<STOP; iterCount++) {
			double f1 = function (x[1]);
			double f0 = function (x[0]);
			
			double x2 = (x[1]-x[0])*f1/(f1-f0) +  x[0];
			double f2 = function(x2);
			
			out << i - brackets.begin() + 1 << "\t" << iterCount+1 << setiosflags(ios::showpoint) << "\t\t" << x2 << "\t\t" << f2 << endl;

			if (f2 == 0 || (x[1] - x[0]) < tolerance) {
				solutions.push_back(x2);
				break;
			}
				
			if (f2*f0 < 0)
				x[1] = x2;
			else
				x[0] = x2;
		}
		
		if (iterCount == STOP) {
			cout << "ERROR: Too many iterations." << endl;
			out.close();
			throw(E2BIG);
		}
	}
	
	out.close();
	return solutions;
}


double function (double x) {
	return (x*x*x + 1.5*x*x - 5.75*x + 4.37);
	// return (x*x-10000*x + 1);
}


double dfunction (double x) {
	return (3*x*x + 3*x - 5.75);
	// return (2*x-10000);
}


double quadPerturbation (double a, double b, double c, double tolerance) {
	int STOP = 50;
	b /= a;
	c /= a;
	double x = 0;
	double dx;
	
	ofstream out;
	out.open("quadratic.log");
	out << "\tTolerance: " << tolerance << endl;
	out << endl;
	out.precision(15);
	out << "Cycle\t\tx\t\t\t\tf(x)" << endl;
	out << "-------------------------------------------------------------" << endl;
	out << 0 << "\t\t" << setiosflags(ios::showpoint) << x << "\t\t" << (x*x + b*x + c) << endl;
	
	for (int i=0; i<STOP; i++) {
		double xnew = -(x*x+c)/b;
		dx = (xnew - x);
		dx = dx>=0 ? dx : -dx;
		
		out << i + 1 << "\t\t" << setiosflags(ios::showpoint) << xnew << "\t\t" << (xnew*xnew + b*xnew + c) << endl;
		
		if (dx < tolerance) {
			out.close();
			return x;
		}
		
		x = xnew;
	}	
	
	cout << "ERROR: Too many iterations." << endl;
	out.close();
	throw(E2BIG);
}


double quadDiscriminant (double a, double b, double c) {
	double x = 2*c/(-b + sqrt(b*b - 4*a*c));
	return x;
}


double quadDiscriminant2 (double a, double b, double c) {
	double x = (-b - sqrt(b*b - 4*a*c))/(2*a);
	return x;
}
