//
// Source file containing the necessary spline interpolation classes for the magnetic
// field interpolation routines (also included some base class material in case
// we want to try other interpolation schemes)
//
// Created March 18, 2013; Overhauled to new, new system August 2013
//
// Brian S. Henderson (bhender1@mit.edu)
//
//

#include "spline_interp.h"
#include <vector>
#include <cmath>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <iostream>

ffInterpolator::ffInterpolator(float *xdata,float *ydata,float *zdata,std::vector<int> info, float scale)
{
	// Unpack values from the info vector
	n = info[0];
	nx = info[1];
	ny = info[2];
	nz = info[3];
	xmin = info[4];
	ymin = info[5];
	zmin = info[6];
	delx = info[7];
	dely = info[8];
	delz = info[9];

//	std::cout<<"\n\n";
//	for (int j=0; j<10; j++) std::cout<<info[j]<<" ";
//	std::cout<<"\n\n";
//	abort();

	// Number of grid boxes in each direction
	bnx = nx-1;
	bny = ny-1;
	bnz = nz-1;

	// One time multiplications and divisions for time saving
	idelx = (float)1/delx;
	idely = (float)1/dely;
	idelz = (float)1/delz;
	nxny = nx*ny;
	xmax = xmin+bnx*delx;
	ymax = ymin+bny*dely;
	zmax = zmin+bnz*delz;
	minc = 12*nxny-24*nx;
	qinc = 6*nx-12;
	tnx = 12*nx;
	tnxny = 24*nx*ny;

	// Hold the data internally
	float * xb =  new float[n];
	float * xdx =  new float[n];
	float * xdy =  new float[n];
	float * xdz =  new float[n];
	float * xdxy =  new float[n];
	float * xdxz =  new float[n];
	float * xdyz =  new float[n];
	float * xdxyz =  new float[n];

	float * yb =  new float[n];
	float * ydx =  new float[n];
	float * ydy =  new float[n];
	float * ydz =  new float[n];
	float * ydxy =  new float[n];
	float * ydxz =  new float[n];
	float * ydyz =  new float[n];
	float * ydxyz =  new float[n];

	float * zb =  new float[n];
	float * zdx =  new float[n];
	float * zdy =  new float[n];
	float * zdz =  new float[n];
	float * zdxy =  new float[n];
	float * zdxz =  new float[n];
	float * zdyz =  new float[n];
	float * zdxyz =  new float[n];

	// Load these coefficients into a unified array that is efficiently ordered
	acof = new float[24*n];

	// Fill the internal arrays, converting to the necessary unit box scale and
	// the magnet current/GEANT4 scale
	for (int j=0; j<n; j++)
	{
		xb[j] = xdata[j]*scale;
		yb[j] = ydata[j]*scale;
		zb[j] = zdata[j]*scale;

		xdx[j] = xdata[j+n]*delx*scale;
		ydx[j] = ydata[j+n]*delx*scale;
		zdx[j] = zdata[j+n]*delx*scale;

		xdy[j] = xdata[j+2*n]*dely*scale;
		ydy[j] = ydata[j+2*n]*dely*scale;
		zdy[j] = zdata[j+2*n]*dely*scale;

		xdz[j] = xdata[j+3*n]*delz*scale;
		ydz[j] = ydata[j+3*n]*delz*scale;
		zdz[j] = zdata[j+3*n]*delz*scale;

		xdxy[j] = xdata[j+4*n]*delx*dely*scale;
		ydxy[j] = ydata[j+4*n]*delx*dely*scale;
		zdxy[j] = zdata[j+4*n]*delx*dely*scale;

		xdxz[j] = xdata[j+5*n]*delx*delz*scale;
		ydxz[j] = ydata[j+5*n]*delx*delz*scale;
		zdxz[j] = xdata[j+5*n]*delx*delz*scale;

		xdyz[j] = xdata[j+6*n]*delz*dely*scale;
		ydyz[j] = ydata[j+6*n]*delz*dely*scale;
		zdyz[j] = zdata[j+6*n]*delz*dely*scale;

		xdxyz[j] = xdata[j+7*n]*delx*dely*delz*scale;
		ydxyz[j] = ydata[j+7*n]*delx*dely*delz*scale;
		zdxyz[j] = zdata[j+7*n]*delx*dely*delz*scale;
	}

	// Fill the final coefficient array in the preferred column ordering
	for (int plane = 0; plane < nz; plane ++)
	{
		for (int col = 0; col < ny; col++)
		{
			for (int j = 0; j < nx; j++)
			{
				acof[12*(col*nx+2*plane*nxny)+6*j]   = xb[col*nx+plane*nxny+j];
				acof[12*(col*nx+2*plane*nxny)+6*j+1] = yb[col*nx+plane*nxny+j];
				acof[12*(col*nx+2*plane*nxny)+6*j+2] = zb[col*nx+plane*nxny+j];
				acof[12*(col*nx+2*plane*nxny)+6*j+3] = xdx[col*nx+plane*nxny+j];
				acof[12*(col*nx+2*plane*nxny)+6*j+4] = ydx[col*nx+plane*nxny+j];
				acof[12*(col*nx+2*plane*nxny)+6*j+5] = zdx[col*nx+plane*nxny+j];

				acof[12*(col*nx+2*plane*nxny)+6*j+6*nx]   = xdy[col*nx+plane*nxny+j];
				acof[12*(col*nx+2*plane*nxny)+6*j+1+6*nx] = ydy[col*nx+plane*nxny+j];
				acof[12*(col*nx+2*plane*nxny)+6*j+2+6*nx] = zdy[col*nx+plane*nxny+j];
				acof[12*(col*nx+2*plane*nxny)+6*j+3+6*nx] = xdxy[col*nx+plane*nxny+j];
				acof[12*(col*nx+2*plane*nxny)+6*j+4+6*nx] = ydxy[col*nx+plane*nxny+j];
				acof[12*(col*nx+2*plane*nxny)+6*j+5+6*nx] = zdxy[col*nx+plane*nxny+j];

				acof[12*(col*nx+2*plane*nxny)+6*j+12*nxny]   = xdz[col*nx+plane*nxny+j];
				acof[12*(col*nx+2*plane*nxny)+6*j+12*nxny+1] = ydz[col*nx+plane*nxny+j];
				acof[12*(col*nx+2*plane*nxny)+6*j+12*nxny+2] = zdz[col*nx+plane*nxny+j];
				acof[12*(col*nx+2*plane*nxny)+6*j+12*nxny+3] = xdxz[col*nx+plane*nxny+j];
				acof[12*(col*nx+2*plane*nxny)+6*j+12*nxny+4] = ydxz[col*nx+plane*nxny+j];
				acof[12*(col*nx+2*plane*nxny)+6*j+12*nxny+5] = zdxz[col*nx+plane*nxny+j];

				acof[12*(col*nx+2*plane*nxny)+6*j+12*nxny+6*nx]   = xdyz[col*nx+plane*nxny+j];
				acof[12*(col*nx+2*plane*nxny)+6*j+12*nxny+1+6*nx] = ydyz[col*nx+plane*nxny+j];
				acof[12*(col*nx+2*plane*nxny)+6*j+12*nxny+2+6*nx] = zdyz[col*nx+plane*nxny+j];
				acof[12*(col*nx+2*plane*nxny)+6*j+12*nxny+3+6*nx] = xdxyz[col*nx+plane*nxny+j];
				acof[12*(col*nx+2*plane*nxny)+6*j+12*nxny+4+6*nx] = ydxyz[col*nx+plane*nxny+j];
				acof[12*(col*nx+2*plane*nxny)+6*j+12*nxny+5+6*nx] = zdxyz[col*nx+plane*nxny+j];
			}
		}
	}

	// Free up the memory from the temp holders
	delete [] xb;
	delete [] xdx;
	delete [] xdy;
	delete [] xdz;
	delete [] xdxy;
	delete [] xdxz;
	delete [] xdyz;
	delete [] xdxyz;

	delete [] yb;
	delete [] ydx;
	delete [] ydy;
	delete [] ydz;
	delete [] ydxy;
	delete [] ydxz;
	delete [] ydyz;
	delete [] ydxyz;

	delete [] zb;
	delete [] zdx;
	delete [] zdy;
	delete [] zdz;
	delete [] zdxy;
	delete [] zdxz;
	delete [] zdyz;
	delete [] zdxyz;
};

ffInterpolator::~ffInterpolator()
{
	// Delete main coefficient array
	delete [] acof;
};

// The main interpolation function
void ffInterpolator::interpolate(double cx, double cy, double cz, double& bx, double& by, double& bz)
{
	// Set the fields to zero to get ready to sum
	bx = 0.0000;
	by = 0.0000;
	bz = 0.0000;

	// If the point is outside the grid, field is set to 0 by immediately
	// returning
	if ((cx>xmax) || (cx<xmin)) return;
	if ((cy>ymax) || (cy<ymin)) return;	
	if ((cz>zmax) || (cz<zmin)) return;

	// Indices of box along each direction
	int ixs, iys, izs;

	// Get the box coordinates in the grid
	ixs = (int)floor((cx-xmin)*idelx);
	iys = (int)floor((cy-ymin)*idely);
	izs = (int)floor((cz-zmin)*idelz);

	// Compute the starting coefficient based on the coordinates
	int ind = 6*ixs+iys*tnx+izs*tnxny;

	// Get the box fractional coordinates
	float x = (cx-xmin-delx*ixs)*idelx;
	float y = (cy-ymin-dely*iys)*idely;
	float z = (cz-zmin-delz*izs)*idelz;

	// Compute the basis functions
	// First some things that show up more than once to save operations
	float xmo = x-1;
	float ymo = y-1;
	float zmo = z-1;
	float xmos = xmo*xmo;
	float ymos = ymo*ymo;
	float zmos = zmo*zmo;
	float xs = x*x;
	float ys = y*y;
	float zs = z*z;
	// The 12 basis functions themselves
	float fx[4] = {xmos*(1+2*x), xmos*x, xs*(3-2*x), xmo*xs};
	float fy[4] = {ymos*(1+2*y), ymos*y, ys*(3-2*y), ymo*ys};	
	float fz[4] = {zmos*(1+2*z), zmos*z, zs*(3-2*z), zmo*zs};

	// Add up the coefficient times basis function terms
	for (int m = 0; m<4; m++, ind += minc)
	{
		float tm = fz[m];
		for (int q = 0; q<4; q++, ind += qinc)
		{
			// Temp values to save multiplications
			float tf = fy[q]*tm;
			float tbx = 0.0000; float tby = 0.0000; float tbz = 0.0000;
			for (int k = 0; k<4; k++)
			{
				float tk = fx[k];
				tbx += acof[ind++]*tk;
				tby += acof[ind++]*tk;
				tbz += acof[ind++]*tk;
			}

			bx += tbx*tf;
			by += tby*tf;
			bz += tbz*tf;
		}
	}

	return;
}
