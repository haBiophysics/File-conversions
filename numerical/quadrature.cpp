// Numerical quadrature evaluation

# include <iostream>
# include <math.h>
# include <asm-generic/errno.h>
# include <fstream>
# include <iomanip>

# define TOLERANCE 1e-10
# define MAX 20

using namespace std;

double romberg (double a, double b, double (*func)(double));
double simpson (double a, double b, double (*f)(double));
double trapezoid (double a, double b, double (*f)(double));
double f1 (double x);
double f2 (double x);
double f3 (double x);
double f4 (double x);
double f5 (double x);
double f6 (double x);
int Q1 (void);
int Q2 (void);
int Q3 (void);
int Q4 (void);


int main (void) {
	Q4();
	Q3();
	Q2();
	Q1();
	
	return 0;
}


int Q1 (void) {
	double a = 0;
	double b = M_PI/2;
	
	cout.precision(15);
	cout << setiosflags(ios::showpoint);
	
	cout << "Q1:" << endl;
	cout << "Trapezoid:\t\t" << trapezoid(a,b, sin) << endl;
	cout << "Simpson:\t\t" << simpson(a,b, sin) << endl;
	cout << "Romberg:\t\t" << romberg(a,b, sin) << endl;
	cout << "Real value:\t\t" << cos(a) - cos(b) << endl;
	return 0;
}

int Q2 (void) {
	double a1 = 0;
	double b1 = M_PI/2;
	double a2 = 3;
	double b2 = 5;
	
	
	cout.precision(15);
	cout << setiosflags(ios::showpoint);
	cout << "Q2:" << endl;	
	cout << "Trapezoid:\t\t" << 2*trapezoid(a1,b1,f6) + trapezoid(a2,b2,f3) << endl;
	cout << "Simpson:\t\t" << 2*simpson(a1,b1,f6) + simpson(a2,b2,f3) << endl;
	cout << "Romberg:\t\t" << 2*romberg(a1,b1,f6) + romberg(a2,b2,f3) << endl;
	cout << "Real value:\t\t" << 2*sqrt(1.5)+2*sqrt(3.5) << endl;

	return 0;
}


double f3 (double x) {
	return pow(fabs(x-1.5), -0.5);
}


double f6 (double x) {
	return 2*sqrt(1.5)*sin(x)*cos(x)/fabs(cos(x));
}


int Q3 (void) {
	double a1 = 0;
	double b1 = 2;
	double a2 = 0;
	double b2 = 0.5;
	
	cout.precision(15);
	cout << setiosflags(ios::showpoint);
	cout << "Q3:" << endl;
	cout << "Trapezoid:\t\t" << trapezoid(a1,b1, f1) + trapezoid(a2,b2, f2) << endl;
	cout << "Simpson:\t\t" << simpson(a1,b1,f1) + simpson(a2,b2,f2) << endl;
	cout << "Romberg:\t\t" << romberg(a1,b1, f1) + romberg (a2,b2,f2) << endl;
	cout << "Real value:\t\t" << M_PI/(2*sqrt(3.0)) << endl;
	return 0;
}


double f1 (double x) {
	return 1/(1+x*x+x*x*x*x);
}


double f2 (double x) {
	return x*x/(1+x*x+x*x*x*x);
}


int Q4 (void) {
	double a1 = 0;
	double b1 = 3*pow(2.0,-1.0/3.0);
	double a2 = 0;
	double b2 = 3*pow(2.0,-5.0/3.0);
	
	cout.precision(15);
	cout << setiosflags(ios::showpoint);
	cout << "Q4:" << endl;	
	cout << "Trapezoid:\t\t" << trapezoid(a1,b1, f4) + trapezoid(a2,b2, f5) << endl;
	cout << "Simpson:\t\t" << simpson(a1,b1,f4) + simpson(a2,b2,f5) << endl;
	cout << "Romberg:\t\t" << romberg(a1,b1, f4) + romberg (a2,b2,f5) << endl;
	cout << "Real value:\t\t" << 2*M_PI/sqrt(3.0) << endl;

	return 0;
}


double f4 (double x) {
	return pow(1.0-x*x*x/27.0, -1.0/3.0);
}


double f5 (double x) {
	return pow(1.0-pow(2.0*x/3.0,3.0/2.0), -2.0/3.0);
}


double romberg (double a, double b, double (*f)(double)) {
	if (a == b) {
		cout << "ERROR: integration domain is a point." << endl;
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
	out << "Cycle\t\t\tf(x)dx" << endl;
	out << "--------------------------------------------------" << endl;
	out.precision(15);
	out << setiosflags(ios::showpoint);
	
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
		
		if (fabs(quads[i][i]-quads[i-1][i-1]) < TOLERANCE && i>5) {
			out.close();
			return quads[i][i];
		}
	}
	
	cout << "ERROR: Romberg did not converge after " << MAX << " iterations." << endl;
	out.close();
	throw(E2BIG);
}


double simpson (double a, double b, double (*f)(double)) {
	if (a == b) {
		cout << "ERROR: Integration domain is a point." << endl;
		throw(EINVAL);
	}
	
	if (a > b) {
		return -simpson(b,a,f);
	}
	
	ofstream out;
	out.open("simpson.log");
	out << "Cycle\t\t\tf(x)dx" << endl;
	out << "--------------------------------------------------" << endl;
	out.precision(15);
	out << setiosflags(ios::showpoint);
	
	double h = (b-a);
	double prev = TOLERANCE;
	for (int j=1; j< MAX; j++) {
		double sum=f(a);
		double N = (b-a)/h;
		for (int i=1; i<N; i++) {
			if (i%2 == 1) {
				sum+=4*f(a+i*h);
			}
			else {
				sum+=2*f(a+i*h);
			}
		}
	
		sum = h/3*(sum+f(b));
		out << j << "\t\t\t" << sum << endl;
		if (fabs(prev-sum) < TOLERANCE && j>5)
			return sum;
		prev = sum;
		h/=2;
	}
	cout << "ERROR: Simpson did not converge after " << MAX << " iterations." << endl;
	out.close();
	throw(E2BIG);
}


double trapezoid (double a, double b, double (*f)(double)) {
	if (a == b) {
		cout << "ERROR: integration interval is a point." << endl;
		throw(EINVAL);
	}
	
	if (a > b) {
		return -trapezoid(b,a,f);
	}
	
	ofstream out;
	out.open("trapezoid.log");
	out << "Cycle\t\t\tf(x)dx" << endl;
	out << "--------------------------------------------------" << endl;
	out.precision(15);
	out << setiosflags(ios::showpoint);
	
	double h = (b-a);
	double x = a;
	double sum = 0;
	
	sum += 0.5*f(x);
	x+=h;
	while (x<b) {
		sum += f(x);
		x+=h;
	}
	double previous = h*(sum+0.5*f(x));
	
	for (int i=1; i<MAX; i++) {
		double newterms=0;
		double x = a+h/2;
		while (x < b) {
			newterms+= f(x);
			x+=h;
		}
		
		double current = previous/2 + newterms*h/2;
		out << i << "\t\t\t" << current << endl;
		if (fabs(current - previous) < TOLERANCE && i>5) {
			out.close();
			return current;
		}
		
		previous = current;
		h /= 2;
	}
	
	cout << "ERROR: Trapezoid did not converge after " << MAX << " iterations." << endl;
	out.close();
	throw(E2BIG);
}
