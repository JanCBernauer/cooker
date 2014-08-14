// Include the Magnetic_Field and other user header files.

#include "Magnetic_Field.h"
#include "spline_interp.h"

// Include the GEANT4 header files referenced here.
#include "globals.hh"
#include "G4ios.hh"

// Include the C++ header files referenced here.
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <iomanip>
#include <cmath>

// Constructor for Spline_Field class.


Spline_Field::Spline_Field( const char * filename, double current) : Magnetic_Field(), magnetCurrent( current )
{
	G4cout << "Magnet Current set to " << magnetCurrent << " A." << G4endl;

	symb = false; // Set no special SYMB field until otherwise found

	read(filename);

	// Get info about the field file that has been read via an MD5 checksum
	fieldCS = TMD5::FileChecksum(filename);
	fieldCS->Final();  // No more will be added to grid, compute hash
	// std::cout<<"\n\n\n"<<fieldCS->AsString()<<"\n\n\n";
}

Spline_Field::~Spline_Field()
{
  delete bint;
}

// Member function to return the magnetic field value at a given point.
void Spline_Field::GetFieldValue( const G4double Point[4],
                                    G4double * Bfield ) const
{
	G4double x = Point[0]/mm;
	G4double y = Point[1]/mm;
	G4double z = Point[2]/mm;

	// cout<<x<<" "<<y<<" "<<z<<"\n";

	// Holders for the field interpolation
	G4double bxh, byh, bzh;

	// If a SYMB grid is present check if it should be used
	if (symb)
	{
		// cout<<"In the SYMB check "<<x<<" "<<y<<" "<<z;
		if (((std::abs(x)<100) && (std::abs(y)<50)) && (z>500 && z<3000))
		{
			symbInt(x,y,z,bxh,byh,bzh);
			// cout<<x<<" "<<y<<" "<<z<<" "<<bxh<<" "<<byh<<" "<<bzh<<" SYMB!\n";
		}
		// Else use the main grid
		else
		{
			bint->interpolate(x,y,z,bxh,byh,bzh);
			// cout<<" "<<bxh<<" "<<byh<<" "<<bzh<<" OUT!\n";
		}
	}
	// Else use the main grid
	else bint->interpolate(x,y,z,bxh,byh,bzh);

	// Fill the outputs
	Bfield[0] = bxh*magnetCurrent;
	Bfield[1] = byh*magnetCurrent;
	Bfield[2] = bzh*magnetCurrent;

	// The electric field is always zero.
	Bfield[3] = 0.0;
	Bfield[4] = 0.0;
	Bfield[5] = 0.0;

}

void Spline_Field::read(const char * filename)
{
	ifstream grid(filename, ifstream::binary);

	// Check if file has been opened successfully:
	if (!grid.is_open())
	 {
		G4cerr << "Cannot open magnetic field grid file >"
		  << filename  << "<, exiting..." << G4endl;
		exit(1);
	 }

	// Read the header as usual
	grid.read((char *)&coords,sizeof(coords));
	grid.read((char *)&fNdata,sizeof(fNdata));
	grid.read((char *)&fNx,sizeof(fNx));
	grid.read((char *)&fNy,sizeof(fNy));
	grid.read((char *)&fNz,sizeof(fNz));
	grid.read((char *)&fXmin,sizeof(double));
	grid.read((char *)&fYmin,sizeof(double));
	grid.read((char *)&fZmin,sizeof(double));
	grid.read((char *)&fDx,sizeof(double));
	grid.read((char *)&fDy,sizeof(double));
	grid.read((char *)&fDz,sizeof(double));

	// Make info vector to push to interpolator
	info.push_back(fNdata);
	info.push_back(fNx);
	info.push_back(fNy);
	info.push_back(fNz);
	info.push_back((int)fXmin);
	info.push_back((int)fYmin);
	info.push_back((int)fZmin);
	info.push_back((int)fDx);
	info.push_back((int)fDy);
	info.push_back((int)fDz);

	// Create the float arrays with the known size info from the header
	float * xdata = new float[8*fNx*fNy*fNz];
	float * ydata = new float[8*fNx*fNy*fNz];
	float * zdata = new float[8*fNx*fNy*fNz];

	// Read them off the deriv.bin grid
	grid.read((char *)xdata,8*sizeof(float)*fNx*fNy*fNz);
	grid.read((char *)ydata,8*sizeof(float)*fNx*fNy*fNz);
	grid.read((char *)zdata,8*sizeof(float)*fNx*fNy*fNz);

	// Look for an appended grid for special handling of the SYMB region
	int sytest;
	grid.read((char *)&sytest,sizeof(sytest));

	// If present, read the appended data
	if (!grid.eof() && sytest==9999)
	{
		cout<<"\nContinuing read of appending SYMB region grid!\n\n";
		symb = true;

		// Continue reading the header line
		grid.read((char *)&sNdata,sizeof(sNdata));
		grid.read((char *)&sNx,sizeof(sNx));
		grid.read((char *)&sNy,sizeof(sNy));
		grid.read((char *)&sNz,sizeof(sNz));
		grid.read((char *)&sXmin,sizeof(double));
		grid.read((char *)&sYmin,sizeof(double));
		grid.read((char *)&sZmin,sizeof(double));
		grid.read((char *)&sDx,sizeof(double));
		grid.read((char *)&sDy,sizeof(double));
		grid.read((char *)&sDz,sizeof(double));

		// Compute some things for multiple-use time saving
		sNxz = sNx*sNz;
		isDx = 1/sDx;
		isDy = 1/sDy;
		isDz = 1/sDz;
	
		// Read the grid into the arrays
		sbx = new float[sNxz];
		sby = new float[sNxz];
		sbz = new float[sNxz];
		grid.read((char *)sbx,sizeof(float)*sNxz);
		grid.read((char *)sby,sizeof(float)*sNxz);
		grid.read((char *)sbz,sizeof(float)*sNxz);

		// cout<<gauss<<"\n";
		// Scale to the silly Geant4 internal units
		for (int j=0; j<sNxz; j++)
		{
			sbx[j] = gauss*sbx[j];
			sby[j] = gauss*sby[j];
			sbz[j] = gauss*sbz[j];
			// cout<<sbx[j]<<" "<<sby[j]<<" "<<sbz[j]<<"\n";
		}

	}
	else if (!grid.eof())
	{
		cout<<"\nWARNING! WARNING! WARNING!\n\nThere is more data in your field grid, but it does not\n match the pattern for an appended SYMB grid!\n\n!";
	}
	else
	{
		cout<<"\nEnd of field grid!\n";
	}
	
	grid.close();

	// Setup the interpolation scheme
	bint = new ffInterpolator(xdata,ydata,zdata,info,gauss);
	
	// All this is no longer needed once spline coefficients are totally setup
	delete [] xdata;
	delete [] ydata;
	delete [] zdata;

//	// Interpolation test code
//	ifstream tps;
//	tps.open("/home/bsh/.cooker/shared/MC/MollerGrid.txt");
//	ofstream otest;
//	otest.open("points_test.txt");
//	double xt, yt, zt, bxt, byt, bzt, bxs, bys, bzs, brs, bts, bxcs, bycs, incur;
//	double point[4];
//	double field[6];
//	otest<< getCurrent() << " A\n";
//	while (!tps.eof())
//	{
//		tps >> xt >> yt >> zt >> bxt >> byt >> bzt >> incur;
//		 bint->interpolate(xt,yt,zt,bxs,bys,bzs);
//		point[0] = xt; point[1] = yt; point[2] = zt;
//		GetFieldValue(point,field);
//		otest << xt << " " << yt << " " << zt << "  "<< 5000*field[0]/gauss << " " << 5000*field[1]/gauss << " " << 5000*field[2]/gauss << " " << bxt*10000 << " " << byt*10000 << " " << bzt*10000 << "\n";
//		 otest << bxt*6730 << " " << byt*6730 << " " << bzt*6730 << "   ";
//		 otest << bxs*6730/5000/gauss << " " << bys*6730/5000/gauss << " " << bzs*6730/5000/gauss << "\n";
//		 if (j%1000 == 0) cout<<"Interpolation test point "<<j<<"\n";
//	}
//	tps.close();
//	otest.close();

//	// Some code to print out the field at some specfied points
//	ifstream wcpts;
//	wcpts.open("/home/bsh/cooker_WC_base/master/testpts.dat");
//	ofstream out;
//	out.open("/home/bsh/cooker_WC_base/master/interpfield.dat");

//	// int wire;
//	double xc, yc, zc; // bxc, byc, bzc;
//	double point[4];
//	double field[6];

//	// out << getCurrent() << " A\n";

//	while (!wcpts.eof())
//	{
//		wcpts >> xc >> yc >> zc;
//		point[0] = xc;
//		point[1] = yc;
//		point[2] = zc;
//		GetFieldValue(point,field);
//		out << field[0]/gauss << " " << field[1]/gauss << " " << field[2]/gauss << "\n";
//	}

//	wcpts.close();
//	out.close();
//   exit(0);
};

// Interpolator for the SYMB region grid
void Spline_Field::symbInt(double qx, double qy, double qz, double& bx, double& by, double& bz) const
{

	// cout<<"IMMA CHARGIN MA LAZER!\n";

	// Determine the grid coordinates
	int ixs = (int)floor((qx-sXmin)*isDx);
	int izs = (int)floor((qz-sZmin)*isDz);

	// Get the indices of the grid points in the field grid
	int naa = izs*sNx+ixs;
	int nba = naa+1;
	int nab = naa+sNx;
	int nbb = nab+1;

	// Some reusable factors
	double f = isDx*isDz;
	double dx = qx-sDx*ixs-sXmin;
	double dz = qz-sDz*izs-sZmin;
	double ux = sDx-dx;
	double uz = sDz-dz;

	// Calculate the field components using bilinear interpolation
	bx = f*(sbx[naa]*ux*uz + sbx[nba]*dx*uz + sbx[nab]*ux*dz + sbx[nbb]*dx*dz);
	by = f*(sby[naa]*ux*uz + sby[nba]*dx*uz + sby[nab]*ux*dz + sby[nbb]*dx*dz);
	bz = f*(sbz[naa]*ux*uz + sbz[nba]*dx*uz + sbz[nab]*ux*dz + sbz[nbb]*dx*dz);

	// cout<<bx<<" "<<by<<" "<<bz<<"\n";
	return;
	
}

// Get functions for private variables
double Spline_Field::setCurrent( double current )
{
	return magnetCurrent = current;
}

double Spline_Field::getCurrent()
{
	return magnetCurrent;
}

bool Spline_Field::symbField()
{
	return symb;
}
