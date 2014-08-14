//
// Header containing the necessary spline interpolation classes for the magnetic
// field interpolation routines (also included some base class material in case
// we want to try other interpolation schemes)
//
// Created March 18, 2013; Overhauled to new new system August 2013
//
// Brian S. Henderson (bhender1@mit.edu)
//
//

#include <vector>
#include <cmath>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <iostream>

using namespace std;

class ffInterpolator
{
	public:

	// Constructor/destructor
	ffInterpolator(float *xdata,float *ydata,float *zdata,std::vector<int> info,float scale);
	~ffInterpolator();
	
	// Main interpolation function
   void interpolate(double cx, double cy, double cz, double& bx, double& by, double& bz);

	private:
	
	// float *xb, *xdx, *xdy, *xdz, *xdxy, *xdxz, *xdyz, *xdxyz,
	//		*yb, *ydx, *ydy, *ydz, *ydxy, *ydxz, *ydyz, *ydxyz,
	//		*zb, *zdx, *zdy, *zdz, *zdxy, *zdxz, *zdyz, *zdxyz;
	float *acof;
	int n, nx, ny, nz, nxny, bnx, bny, bnz, xmin, ymin, zmin, xmax, ymax, zmax, delx, dely, delz;
	float idelx, idely, idelz;
	//	int findBox(double x, double y, double z); //, int * p);
	int minc, qinc;
	int tnx, tnxny;

	// OLD FUNCTIONS FROM FIRST SCHEME
	// void findPoints(int ix, int iy, int iz); //, int * p);
	// int lookupStartInd(int j);
	// void loadCoefs(int * p, float * cof); // x, float * cofy, float * cofz);
	// std::vector< vector<float> > copt; NO LONGER NEEDED IN NEW SCHEME, LEFT FOR CLARITY
	// void genBoxCoef(int box); NO LONGER NEEDED IN NEW SCHEME, LEFT FOR CLARITY

};
