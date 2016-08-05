// Tunc Kayikcioglu, 2016
// Ha Lab, UIUC

// Gaussian peak fitting to a bright spot in a small frame
// There should be one and only one spot within the provided subframe.
// A large image needs to be cropped to chunks to ensure this.

// To gain better understanding about the usage of GSL solver, please refer to the following example:
// http://www.csse.uwa.edu.au/programming/gsl-1.0/gsl-ref_35.html

#include <stdio.h>
#include <iostream>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_multifit_nlin.h>

using namespace std;


void print_state (size_t iter, gsl_multifit_fdfsolver* s) {
	gsl_vector* params = s->x;
	printf ("iter: %3u x = ", (unsigned int) iter);
	for (int i=0; i<params->size; i++)
		printf("%15.8f ", gsl_vector_get(params,i));
	
	printf("|f(x)| = %g\n", gsl_blas_dnrm2 (s->f));
	return;
}


struct frame {
	size_t n; // Num pixels in the frame, frames are assumed to be square
	double* z; // Intensity readings in the n x n pixels.
	frame (int size) {
		n = size*size;
		z = new double[n];
	}
	
	frame (const frame& other) {
		n = other.n;
		z = new double[n];
		for (int i=0; i<n; i++)
			z[i] = other.z[i];
	}
	
	frame& operator= (const frame& other){
		if (this != &other) {
			n = other.n;
			for (int i=0; i<n; i++)
				z[i] = other.z[i];
		}
		return *this;
	}
	
	~frame() {
		delete[] z;
		z = NULL;
	}
};


// Gaussian 2D intensity profile
// I = b^2 + A*exp(-(x-x0)^2/2*sx^2 - (y-y0)^2/2*sy^2)
// J: Jacobian of the Gaussian with respect to fitting parameters (i.e. x0, sx ...)
int twoDgaussian_fdf (const gsl_vector* x, void* params, gsl_vector* f, gsl_matrix* J) {
	size_t n = ((struct frame*)params)->n;
	int dim = sqrt(n); // Dimension of the frame (assumed square)
	double* z = ((struct frame*)params)->z;
	
	// Fitting parameters
	double b = gsl_vector_get (x, 0);
	double A = gsl_vector_get (x, 1);
	double x0 = gsl_vector_get (x, 2);
	double sx = gsl_vector_get (x, 3); // sx = sx*sx*2;
	double y0 = gsl_vector_get (x, 4);
	double sy = gsl_vector_get (x, 5); // sy = sy*sy*2;
	for (int i = 0; i<n; i++) {
		double dx = i/dim + 0.5 - x0;
		double dy = (i%dim) + 0.5 - y0;
		double e = exp(-pow(dx,2)/sx-pow(dy,2)/sy);
		double Zi = b + A*e;
		gsl_vector_set (f, i, Zi - z[i]); // residual of the fitting
		gsl_matrix_set (J, i, 0, 1);
		gsl_matrix_set (J, i, 1, e);
		gsl_matrix_set (J, i, 2, 2*A*e*dx/sx);
		gsl_matrix_set (J, i, 3, A*pow(dx/sx,2)*e);
		gsl_matrix_set (J, i, 4, 2*A*e*dy/sy);
		gsl_matrix_set (J, i, 5, A*pow(dy/sy,2)*e);
	}
	return GSL_SUCCESS;
}


int twoDgaussian_f (const gsl_vector* x, void *params, gsl_vector* f) {
	size_t n = ((struct frame*)params)->n;
	gsl_matrix* J = gsl_matrix_alloc (n, 6);
	twoDgaussian_fdf (x, params, f, J);
	gsl_matrix_free (J);
	return GSL_SUCCESS;
}


int twoDgaussian_df (const gsl_vector* x, void *params, gsl_matrix* J) {
	size_t n = ((struct frame*)params)->n;
	gsl_vector* f = gsl_vector_alloc (n);
	twoDgaussian_fdf (x, params, f, J);
	gsl_vector_free (f);
	return GSL_SUCCESS;
}


int twoDgaussFit (frame& data, double* p, double dxtol=1e-8, int maxiter=500) {
	const gsl_multifit_fdfsolver_type *T;
	T = gsl_multifit_fdfsolver_lmsder;
	
	gsl_multifit_function_fdf f;
	f.fdf = &twoDgaussian_fdf;
	f.f = &twoDgaussian_f;
	f.df = &twoDgaussian_df;
	f.n = data.n;
	f.p = 6; // number of degrees of freedom to be adjusted, fixed
	f.params = &data;
	
	gsl_multifit_fdfsolver *s;
	s = gsl_multifit_fdfsolver_alloc (T, f.n, f.p);

	gsl_vector_view pvector = gsl_vector_view_array (p, f.p);
	gsl_multifit_fdfsolver_set (s, &f, &pvector.vector);
	
	int status;
	int iter = 0;
	//print_state (iter, s);
	do {
		iter++;
		status = gsl_multifit_fdfsolver_iterate (s);
		//printf ("status = %s\n", gsl_strerror (status));
		//print_state (iter, s);
		if (status)
			break;
		status = gsl_multifit_test_delta (s->dx, s->x, dxtol, dxtol);
	}
	while (status == GSL_CONTINUE && iter < maxiter);
	
	for (int i=0; i<f.p; i++)
		p[i] = gsl_vector_get(s->x,i);

	gsl_multifit_fdfsolver_free (s);
	return 0;
}


class peak {
	public:
		float xpos,ypos;
		float xspread, yspread;
		float intensity, background;
		bool isGood;
		
		peak();
		peak(float xpos_, float ypos_, float xspread_, float yspread_, float intensity_, float background_, float isGood);
};


peak::peak(void) {
	peak(-1,-1,-1,-1, -1, -1, true);
}


peak::peak(float xpos_, float ypos_, float xspread_, float yspread_, float intensity_, float background_, float isGood_) {
	xpos = xpos_;
	ypos = ypos_;
	xspread = xspread_;
	yspread = yspread_;
	isGood = isGood_;
	intensity = intensity_;
	background = background_;
}


peak gaussFitting (const vector <vector <float> >& input) {
	// Allocate gsl_vector to be filled by the std:vector matrix
	int size = input.size(); // We are assuming a square input
	struct frame data (size);
	
	// First find estimates of the parameters (by averaging)
	// We hope to speed up the convergence by providing reasonable initial guesses.
	float xsum=0; // Average x position
	float ysum=0;
	float Itot = 0; // Total intensity in this frame
	float x2sum=0; // variance in x
	float y2sum=0;
	float maxInt=0;
	for (int i=0; i<size; i++)
		for (int j=0; j<size; j++) {
			float current = input[i][j];
			xsum += current*i;
			ysum += current*j;
			x2sum += current*i*i;
			y2sum += current*j*j;
			Itot += current;
			data.z[i*size+j] = current;
			if (current > maxInt)
				maxInt = current;
		}
	
	// Vector of parameters for initial iteration
	double* p = (double*) malloc(6*sizeof(double));
	p[0] = 0.1*Itot/(size*size); // initial background estimate
	p[1] = maxInt - p[0]; // A
	p[2] = xsum/Itot+0.5; // <Xo>
	p[3] = 2*(x2sum/Itot-pow(xsum/Itot,2)); // 2sx^2
	p[4] = ysum/Itot+0.5; // <Yo>
	p[5] = 2*(y2sum/Itot-pow(ysum/Itot,2)); // 2sy^2
	
	// Invoke a gaussian fitting by Levenberg-Marquardt algorithm here.
	twoDgaussFit (data, p);
	
	// Return the fit parameters in a peak element
	peak out;
	out.background = p[0];
	out.intensity = p[1]; // A
	out.xpos = p[2]; // <x>
	out.ypos = p[4]; // <y>
	out.xspread = sqrt(p[3]/2); // std(x)
	out.yspread = sqrt(p[5]/2); // std(y)
	out.isGood = true;
	
	free(p);
	return out;
}
