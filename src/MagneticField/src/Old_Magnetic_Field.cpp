//! \file
//!
//! Source file for Magnetic_Field class.
//!
//! This defines the Magnetic_Field class and the member routines which
//! construct the COOKER toroidal magnetic field.
//!
//! \author D.K. Hasell
//! \version 1.0
//! \date 2010-10-14
//!
//! \ingroup detector

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
#include "printfcolors.h"
#include <iomanip>
#include <cmath>

int counter =0;

// Constructor for Magnetic_Field class.
Old_Magnetic_Field::Old_Magnetic_Field( const char * filename, G4double scale) : Magnetic_Field(), fScale( scale )
{
	//G4cout << " Reading field grid .. \n" << flush;
	G4cout << " BField scale set to " << fScale*100 << "%" << G4endl;
	ffMode = false;

	// Call the appropriate read routine based on the file type
	if (strcmp((filename+(strlen(filename)-10)),".deriv.bin")==0) // Check for new spline grid type
	{
		std::cout<<"\nReading the derivative binary file "<<filename<<" ...\n\n";
		readDerivBinFile(filename);
		ffMode = true;
	}
	else if (strcmp((filename+(strlen(filename)-4)),".bin")==0) // check if the file provided is a binary file or a text file
	{
		std::cout<<"\nReading the binary file field grid "<<filename<<" ...\n\n";
		readBinFile(filename);
	}
	else
	{
		std::cout<<"\nReading a text file field grid "<<filename<<" ...\n\n";
		readTextFile(filename);
	}
	// Get info about the field file that has been read via an MD5 checksum
	fieldCS = TMD5::FileChecksum(filename);
	fieldCS->Final();  // No more will be added to grid, compute hash
	// cout<<"\n\n\n"<<fieldCS->AsString()<<"\n\n\n";
	
	// Run the spline interpolation setup
	// setupSpline();

	/*	ofstream test;
	test.open("gridok.txt");
	test<<coords<<" "<<fNdata<<" "<<fNx<<" "<<fNy<<" "<<fNz<<" "<<fXmin<<" "<<fYmin<<" "<<fZmin<<" "<<fDx<<" "<<fDy<<" "<<fDz;
	for (int j=0; j<(fNx*fNy*fNz); j++)
	{
		test<<"\n"<<pfBx[j]<<" "<<pfBy[j]<<" "<<pfBz[j];
	}
	test.close();
	abort();
	*/
}

// Member function to return the magnetic field value at a given point.
void Old_Magnetic_Field::GetFieldValue( const G4double Point[4],
                                    G4double * Bfield ) const {

  G4double x = Point[0];
  G4double y = Point[1];
  G4double z = Point[2];

  // Declare fractions of interval.

	// Utilities for fiddling with the field in different regions for MC studies
	G4int box = 1; // Dimension in number of grid boxes to use for interpolation
					  // 1: Normal, use nearest grid points
					  // 2: Effectively double the grid size (ignore every other point in each direction)
					  // 3: Effectively triple the grid size
					  // Code not designed to work for other values

	G4int slice = 0;  // Which of 10 slices in global x you want to apply "scale" to
						   // 0: Don't apply a scale to any slice
						   // Any integer k=1-10: Apply the scale to slice k
						   // -1: Apply a more complicated scheme specified below
	G4double scale = 1.00; // Scale applied to the field in the specified slice
							 // (e.g. scale = 1.01 applies a 1% increase to the field in the slice)

	G4double maxx = 2505.6; // Maximum x distance of any ToF from the beamline (mm)

	// Check to see if we are in the slice of interest and compute the final scale to be applied
	G4double slscalex = 1.00;
	G4double slscaley = 1.00;
	G4double slscalez = 1.00;
	
	G4double shiftx = 0.0;
	G4double shifty = 0.0;
	G4double shiftz = 0.0;

	if ((slice == -1))
	{
		if (z > 3000) shifty = 0.05;
		else if (x<200 && x>-200)  shifty = 0.10;
		else shifty = -0.01;
	}
	else if ((x >= (slice-1)*maxx/10) && (x < slice*maxx/10))
	{
		slscalex = scale;
		slscaley = scale;
		slscalez = scale;
		//std::cout<<"x = "<<x<<" Input Scale: "<<fScale<<"\n";
	}

  // Check to make sure the specified grid spacing is OK
  if (box<1 || box>3)
  {
		std::cout<<"\n*****************************************************************\n\n";
		std::cout<<"  Error in Old_Magnetic_Field::GetFieldValue\n\n";
		std::cout<<"  The interpolation grid scale must be 1, 2, or 3!\n\n";
		std::cout<<"  Aborting the magnetic field calculation!\n\n";
		std::cout<<"*****************************************************************\n\n";
	
	   abort();
  }

  // LINEAR INTERPOLATION METHOD
  G4double ffx0, ffx1, ffy0, ffy1, ffz0, ffz1;
 
  ffx0=(x - fXmin ) * fiDx;
  ffy0=(y - fYmin ) * fiDy;
  ffz0=(z - fZmin ) * fiDz;


  G4int ix0=floor(ffx0);
  G4int iy0=floor(ffy0);
  G4int iz0=floor(ffz0);
  
  G4int base=ix0+iy0*fNx+iz0*fNxy; 
   
  // Test that point is within the range of the grid.
  if( ix0 >= 0 && ix0 < fNx-1 &&
			iy0 >= 0 && iy0 < fNy-1 &&
					  iz0 >= 0 && iz0 < fNz-1 )
    {
      ffx0-=ix0;
      ffy0-=iy0;
      ffz0-=iz0;

      ffx1=1-ffx0;
      ffy1=1-ffy0;
      ffz1=1-ffz0;
           
      double f=ffz1*ffy1;
      
      double x1,x2,y1,y2,z1,z2;
      
      x1= f*pfBx[base];    y1= f*pfBy[base];   z1= f*pfBz[base]; 

      base++;
      x2= f*pfBx[base];  y2= f*pfBy[base]; z2= f*pfBz[base];
      f=ffz1*ffy0;
      base+=fNx;
      x2+=f*pfBx[base]; y2+=f*pfBy[base]; z2+=f*pfBz[base];
      base--;
      x1+=f*pfBx[base];   y1+=f*pfBy[base];   z1+=f*pfBz[base];   
      
      f=ffz0*ffy0;
      base+=fNxy;
      x1+=f*pfBx[base];  y1+=f*pfBy[base]; z1+=f*pfBz[base];
      base++;
      x2+=f*pfBx[base]; y2+=f*pfBy[base]; z2+=f*pfBz[base];
      
      f=ffz0*ffy1;
      base-=fNx;
      x2+=f*pfBx[base];y2+=f*pfBy[base];z2+=f*pfBz[base];
      base--;
      x1+=f*pfBx[base];y1+=f*pfBy[base];z1+=f*pfBz[base];
      
      Bfield[0]=x1*ffx1+x2*ffx0;
      Bfield[1]=y1*ffx1+y2*ffx0;
      Bfield[2]=z1*ffx1+z2*ffx0;
  }
  else
    {
      Bfield[0] = 0.0;
      Bfield[1] = 0.0;
      Bfield[2] = 0.0;
    }
  
//**+****1****+****2****+****3****+****4****+****5****+****6****+****7****+****
//
// Scale field value  !!!!!!!!!!!!!!!!!!!!!!!!!
//
// NOTE the minus sign in the following three lines of code are needed so the
// magnet polarity in the experiment has the same sign as the scaling factor
// used in the Monte Carlo.  Specifically a positive magnet polarity in the
// COOKER experiment causes a positively charge particle to bend away from the
// beamline.  With this change a positive magnet scaling factor will result in
// positively charged particles bending away from the beamline also in the
// Monte Carlo.
//
//                        - D.K. Hasell 2012.02.16
//
//**+****1****+****2****+****3****+****4****+****5****+****6****+****7****+****

  Bfield[0] *= -fScale;
  Bfield[1] *= -fScale;
  Bfield[2] *= -fScale;
  
  /*
   G4cout << "\nPosition " << x << " " << z << " " << z << G4endl;
   G4cout << "Old field " << Bfield[0] << " " << Bfield[1] << " " << Bfield[2] 
          << G4endl;

   G4double phi = atan2( y, x );
   G4double Bphi = -Bfield[0] * sin( phi ) + Bfield[1] * cos( phi );

   Bfield[0] = -Bphi * sin( phi );
   Bfield[1] =  Bphi * cos( phi );
   Bfield[2] = 0.0;

   G4cout << "Phi " << phi << " " << Bphi << " " << G4endl; 
   G4cout << "New field " << Bfield[0] << " " << Bfield[1] << " " << Bfield[2] 
          << G4endl;

  */
// Get rid of r and z components (only phi component should survive)
	// to get a field with perfect cylindrical symmetry:
        //G4double phi = atan2(y,x);
	//Btot is total mag. field which is B phi
	//G4double Btot = -Bfield[0]*sin(phi)+Bfield[1]*cos(phi);

	//newly found mag. field components
	//G4double bxnew =-Btot*sin(phi);
	//G4double bynew =Btot*cos(phi);
	//G4double bznew =0.00;

	//then I print out the results
	//	printf("before: Bfield[0]: %f Bfield[1]: %f phi: %f Btot: %f bxnew: %f bynew: %f fScale: %f\n", Bfield[0],Bfield[1],phi,Btot,bxnew,bynew,fScale);

	//Bfield[0] = bxnew;
	//Bfield[1] = bynew;
	//Bfield[2] = bznew;
	
//printf("after: Bfield[0]: %f Bfield[1]: %f phi: %f Btot: %f bxnew: %f bynew: %f fScale: %f\n", Bfield[0],Bfield[1],phi,Btot,bxnew,bynew,fScale);

   

	// This is a pure magnetic field so set electric field to zero just in case.

	Bfield[3] = 0.0;
	Bfield[4] = 0.0;
	Bfield[5] = 0.0;

	//cout << "token " << counter << " " << x << " " << y << " " << z << " " 
	//<< Bfield[0]/gauss << " " << Bfield[1]/gauss  << " " << Bfield[2]/gauss << G4endl;
  counter ++;

  return;
}

Old_Magnetic_Field::~Old_Magnetic_Field()
{
	delete [] pfBx;
	delete [] pfBy;
	delete [] pfBz;
	delete bxint;
	delete byint;
	delete bzint;

//  delete [] Brx;
//  delete [] Bry;
//  delete [] Brz;
//  delete [] Brr;
//  delete [] Brt;
//  delete [] rx;
//  delete [] ry;
//  delete [] rz;
//  delete [] cphi;
//  delete [] sphi;
//  delete xint;
//  delete yint;
//  delete zint;
//  delete rint;
//  delete tint;
}

void Old_Magnetic_Field::readTextFile(const char * filename)
{
  ifstream grid( filename );
  
  // Check if file has been opened successfully:
  if (!grid.is_open())
    {
      G4cerr << ERROR_MSG << "Cannot open magnetic field grid file >"
	     << filename  << "<, exiting..." << NORMAL << G4endl;
      exit(1);
    };
  
  // Read the number of steps, starting value, and step size for grid.
  grid >> coords >> fNdata >> fNx >> fNy >> fNz
       >> fXmin >> fYmin >> fZmin
       >> fDx >> fDy >> fDz;
  
  // std::cout<<"\n\nRead the header, fDz = "<<fDz<<"\n\n";

  // Convert to proper units.
  fXmin *= millimeter;
  fYmin *= millimeter;
  fZmin *= millimeter;
  
  fDx *= millimeter;
  fDy *= millimeter;
  fDz *= millimeter;
  
  fiDx =1/fDx;
  fiDy =1/fDy;
  fiDz =1/fDz;
  
  fNxy=fNx*fNy;

  // Create the arrays in which to store the magnetic field.
  pfBx = new G4double[fNx*fNy*fNz];
  pfBy = new G4double[fNx*fNy*fNz];
  pfBz = new G4double[fNx*fNy*fNz];

  
  // Read field data and fill the arrays with field components in proper units.
  double bx, by, bz;
  int count = 1;
  for( G4int iz = 0; iz < fNz; ++iz )
    {
      for( G4int iy = 0; iy < fNy; ++iy )
	{      
	  for( G4int ix = 0; ix < fNx; ++ix )
	    {
	      grid >> bx >> by >> bz;
			// std::cout<<"Read line "<<count<<", Bx = "<<bx<<", By = "<<by<<", Bz = "<<bz<<"\n";
	      pfBx[ix+fNx*iy+fNxy*iz]=bx*gauss; //other layouts might be better cache aligned....
	      pfBy[ix+fNx*iy+fNxy*iz]=by*gauss;
	      pfBz[ix+fNx*iy+fNxy*iz]=bz*gauss;
			count++;
	    }
	}
      if (iz%(fNz/50)==0) G4cout << "." << flush;
    }
  G4cout << endl;

  // Close data file.
  grid.close();

  // Save a binary file based on the text file that was provided
  char newName[1024];
  strcpy(newName,filename);
  strcat(newName,".bin");
  save(newName);
}

void Old_Magnetic_Field::readBinFile(const char * filename)
{
  ifstream grid( filename ,ifstream::binary);
  
  // Check if file has been opened successfully:
  if (!grid.is_open())
    {
      G4cerr << ERROR_MSG << "Cannot open magnetic field grid file >"
	     << filename  << "<, exiting..." << NORMAL << G4endl;
      exit(1);
    }

  grid.read((char *)&coords,sizeof(coords));
  grid.read((char *)&fNdata,sizeof(fNdata));
  grid.read((char *)&fNx,sizeof(fNx));
  grid.read((char *)&fNy,sizeof(fNy));
  grid.read((char *)&fNz,sizeof(fNz));
  grid.read((char *)&fXmin,sizeof(fXmin));
  grid.read((char *)&fYmin,sizeof(fYmin));
  grid.read((char *)&fZmin,sizeof(fZmin));
  grid.read((char *)&fDx,sizeof(fDx));
  grid.read((char *)&fDy,sizeof(fDy));
  grid.read((char *)&fDz,sizeof(fDz));
  pfBx=new G4double[fNx*fNy*fNz];
  pfBy=new G4double[fNx*fNy*fNz];
  pfBz=new G4double[fNx*fNy*fNz];
  fNxy=fNx*fNy;
  grid.read((char *)pfBx,sizeof(G4double)*fNxy*fNz);
  grid.read((char *)pfBy,sizeof(G4double)*fNxy*fNz);
  grid.read((char *)pfBz,sizeof(G4double)*fNxy*fNz);
  grid.close();
  fiDx =1/fDx;
  fiDy =1/fDy;
  fiDz =1/fDz;  
}

void Old_Magnetic_Field::readDerivBinFile(const char * filename)
{
	ifstream grid(filename, ifstream::binary);

	// Check if file has been opened successfully:
   if (!grid.is_open())
   {
   	G4cerr << ERROR_MSG << "Cannot open magnetic field grid file >"
	     << filename  << "<, exiting..." << NORMAL << G4endl;
      exit(1);
   }

	// int fXmini, fYmini, fZmini, fDxi, fDyi, fDzi;

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

//	fXmin = (double)fXmini;
//	fYmin = (double)fYmini;
//	fZmin = (double)fZmini;
//	fDx = (double)fDxi;
//	fDy = (double)fDyi;
//	fDz = (double)fDzi;

	// Doug's utility numbers for compatibility
	fNxy=fNx*fNy;
	fiDx =1/fDx;
	fiDy =1/fDy;
	fiDz =1/fDz;

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
	float * abx = new float[fNx*fNy*fNz];
	float * abxdx = new float[fNx*fNy*fNz];
	float * abxdy = new float[fNx*fNy*fNz];
	float * abxdz = new float[fNx*fNy*fNz];
	float * abxdxy = new float[fNx*fNy*fNz];
	float * abxdxz = new float[fNx*fNy*fNz];
	float * abxdyz = new float[fNx*fNy*fNz];
	float * abxdxyz = new float[fNx*fNy*fNz];
	float * aby = new float[fNx*fNy*fNz];
	float * abydx = new float[fNx*fNy*fNz];
	float * abydy = new float[fNx*fNy*fNz];
	float * abydz = new float[fNx*fNy*fNz];
	float * abydxy = new float[fNx*fNy*fNz];
	float * abydxz = new float[fNx*fNy*fNz];
	float * abydyz = new float[fNx*fNy*fNz];
	float * abydxyz = new float[fNx*fNy*fNz];
	float * abz = new float[fNx*fNy*fNz];
	float * abzdx = new float[fNx*fNy*fNz];
	float * abzdy = new float[fNx*fNy*fNz];
	float * abzdz = new float[fNx*fNy*fNz];
	float * abzdxy = new float[fNx*fNy*fNz];
	float * abzdxz = new float[fNx*fNy*fNz];
	float * abzdyz = new float[fNx*fNy*fNz];
	float * abzdxyz = new float[fNx*fNy*fNz];

	// Read them off the deriv.bin grid
	grid.read((char *)abx,sizeof(float)*fNx*fNy*fNz);
   grid.read((char *)abxdx,sizeof(float)*fNx*fNy*fNz);
   grid.read((char *)abxdy,sizeof(float)*fNx*fNy*fNz);
   grid.read((char *)abxdz,sizeof(float)*fNx*fNy*fNz);
   grid.read((char *)abxdxy,sizeof(float)*fNx*fNy*fNz);
   grid.read((char *)abxdxz,sizeof(float)*fNx*fNy*fNz);
   grid.read((char *)abxdyz,sizeof(float)*fNx*fNy*fNz);
   grid.read((char *)abxdxyz,sizeof(float)*fNx*fNy*fNz);

   grid.read((char *)aby,sizeof(float)*fNx*fNy*fNz);
   grid.read((char *)abydx,sizeof(float)*fNx*fNy*fNz);
   grid.read((char *)abydy,sizeof(float)*fNx*fNy*fNz);
   grid.read((char *)abydz,sizeof(float)*fNx*fNy*fNz);
   grid.read((char *)abydxy,sizeof(float)*fNx*fNy*fNz);
   grid.read((char *)abydxz,sizeof(float)*fNx*fNy*fNz);
   grid.read((char *)abydyz,sizeof(float)*fNx*fNy*fNz);
   grid.read((char *)abydxyz,sizeof(float)*fNx*fNy*fNz);

   grid.read((char *)abz,sizeof(float)*fNx*fNy*fNz);
   grid.read((char *)abzdx,sizeof(float)*fNx*fNy*fNz);
   grid.read((char *)abzdy,sizeof(float)*fNx*fNy*fNz);
   grid.read((char *)abzdz,sizeof(float)*fNx*fNy*fNz);
   grid.read((char *)abzdxy,sizeof(float)*fNx*fNy*fNz);
   grid.read((char *)abzdxz,sizeof(float)*fNx*fNy*fNz);
   grid.read((char *)abzdyz,sizeof(float)*fNx*fNy*fNz);
   grid.read((char *)abzdxyz,sizeof(float)*fNx*fNy*fNz);
	
	grid.close();

	// Fill the old structures so you can still use linear interpolation
	pfBx = new G4double[fNx*fNy*fNz];
	pfBy = new G4double[fNx*fNy*fNz];
	pfBz = new G4double[fNx*fNy*fNz];

	grid.read((char *)&coords,sizeof(coords));
	grid.read((char *)&fNdata,sizeof(fNdata));
	grid.read((char *)&fNx,sizeof(fNx));
	grid.read((char *)&fNy,sizeof(fNy));
	grid.read((char *)&fNz,sizeof(fNz));
	grid.read((char *)&fXmin,sizeof(fXmin));
	grid.read((char *)&fYmin,sizeof(fYmin));
	grid.read((char *)&fZmin,sizeof(fZmin));
	grid.read((char *)&fDx,sizeof(fDx));
	grid.read((char *)&fDy,sizeof(fDy));
	grid.read((char *)&fDz,sizeof(fDz));

	// Setup for the linear interpolation

	// ofstream test;
	// test.open("gridok.txt");
	// test<<coords<<" "<<fNdata<<" "<<fNx<<" "<<fNy<<" "<<fNz<<" "<<fXmin<<" "<<fYmin<<" "<<fZmin<<" "<<fDx<<" "<<fDy<<" "<<fDz;
	for (int j=0; j<(fNx*fNy*fNz); j++)
	{
		pfBx[j] = (double)((-6730)*gauss*abx[j]);
		pfBy[j] = (double)((-6730)*gauss*aby[j]);
		pfBz[j] = (double)((-6730)*gauss*abz[j]);
		// test<<"\n"<<pfBx[j]*-6730<<" "<<pfBy[j]*-6730<<" "<<pfBz[j]*-6730;
	}
	// test.close();
	// abort();	

	// Setup the interpolation scheme
	// bxint = new ffInterpolator(abx,abxdx,abxdy,abxdz,abxdxy,abxdxz,abxdyz,abxdxyz,info);
	// byint = new ffInterpolator(aby,abydx,abydy,abydz,abydxy,abydxz,abydyz,abydxyz,info);
	// bzint = new ffInterpolator(abz,abzdx,abzdy,abzdz,abzdxy,abzdxz,abzdyz,abzdxyz,info);
	
	// All this is no longer needed once spline coefficients are totally setup
	delete [] abx;
	delete [] abxdx;
	delete [] abxdy;
	delete [] abxdz;
	delete [] abxdxy;
	delete [] abxdxz;
	delete [] abxdyz;
	delete [] abxdxyz;
	delete [] aby;
	delete [] abydx;
	delete [] abydy;
	delete [] abydz;
	delete [] abydxy;
	delete [] abydxz;
	delete [] abydyz;
	delete [] abydxyz;
	delete [] abz;
	delete [] abzdx;
	delete [] abzdy;
	delete [] abzdz;
	delete [] abzdxy;
	delete [] abzdxz;
	delete [] abzdyz;
	delete [] abzdxyz;

//	// Interpolation test code
//	ifstream tps;
//	tps.open("/home/bsh/.cooker/shared/MC/testpoints.grid");
//	ofstream otest;
//	otest.open("points_test.txt");
//	double xt, yt, zt, bxt, byt, bzt, bxs, bys, bzs, brs, bts, bxcs, bycs;
//	double point[4];
//	double field[6];
//	for (int j=0; j<10000; j++)
//	{
//		tps >> xt >> yt >> zt >> bxt >> byt >> bzt;
//		bxs = bxint->interpolate(xt,yt,zt);
//		bys = byint->interpolate(xt,yt,zt);
//		bzs = bzint->interpolate(xt,yt,zt);
//		point[0] = xt; point[1] = yt; point[2] = zt;
//		GetFieldValue(point,field);
//		otest << xt << " " << yt << " " << zt << "   ";
//		otest << bxt*6730/5000 << " " << byt*6730/5000 << " " << bzt*6730/5000 << "   ";
//		otest << bxs*6730 << " " << bys*6730 << " " << bzs*6730 << "   ";
//		otest << field[0]/gauss/0.742942 << " " << field[1]/gauss/0.742942 << " " << field[2]/gauss/0.742942 << "\n";

//		if (j%1000 == 0) cout<<"Interpolation test point "<<j<<"\n";
//	}
//	tps.close();
//	otest.close();
	
};

//// Spline setup
//void Old_Magnetic_Field::setupSpline()
//{
//	// Setup for spline interpolation

//	// Create axisInfo objects for passing to the interpolation functions
//	axisInfo xinfo = axisInfo(fXmin,fXmin+(fNx-1)*fDx,fNx,fDx);
//	axisInfo yinfo = axisInfo(fYmin,fYmin+(fNy-1)*fDy,fNy,fDy);
//	axisInfo zinfo = axisInfo(fZmin,fZmin+(fNz-1)*fDz,fNz,fDz);

//	// Allocation for the double arrays

//	// Coordinate values
//	rx = new double[fNx*fNy*fNz];
//	ry = new double[fNx*fNy*fNz];
//	rz = new double[fNx*fNy*fNz];
//	cphi = new double[fNx*fNy*fNz];
//	sphi = new double[fNx*fNy*fNz];

//	// Real only B field arrays (Cartesian and cylindrical)
//	Brx = new double[fNx*fNy*fNz];
//	Bry = new double[fNx*fNy*fNz];
//	Brz = new double[fNx*fNy*fNz];
//	Brr = new double[fNx*fNy*fNz];
//	Brt = new double[fNx*fNy*fNz];

//	// Load these arrays based on the data read in already
//	for( G4int iz = 0; iz < fNz; ++iz )
//	{
//		for( G4int iy = 0; iy < fNy; ++iy )
//		{
//			for( G4int ix = 0; ix < fNx; ++ix )
//			{
//				// Compute the current index for convenience
//				int a = iz+fNz*iy+fNz*fNy*ix;

//				// Fill Cartesian array by reversing initial array to row-major
//				// standard
//				Brx[a] = pfBx[ix+fNx*iy+fNxy*iz];
//				Bry[a] = pfBy[ix+fNx*iy+fNxy*iz];
//				Brz[a] = pfBz[ix+fNx*iy+fNxy*iz];

//				// Position coordinates
//				rx[a] = fXmin+ix*fDx;
//				ry[a] = fYmin+iy*fDy;
//				rz[a] = fZmin+iz*fDz;

//				// One-time cylindrical angle calculation for performance
//				double phi = atan2(ry[a],rx[a]);
//				cphi[a] = cos(phi);
//				sphi[a] = sin(phi);

//				// Compute the cylindrical components based on the Cartesian values
//				// and the position
//				Brr[a] = Brx[a]*cphi[a] + Bry[a]*sphi[a];
//				Brt[a] = -Brx[a]*sphi[a] + Bry[a]*cphi[a];
//  			}
//		}
//	}

//	// Create the spline interpolation objects for each coordinate direction
//	xint =  new gridInterp(rx,ry,rz,Brx,xinfo,yinfo,zinfo);
//	yint =  new gridInterp(rx,ry,rz,Bry,xinfo,yinfo,zinfo);
//	zint =  new gridInterp(rx,ry,rz,Brz,xinfo,yinfo,zinfo);
//	rint =  new gridInterp(rx,ry,rz,Brr,xinfo,yinfo,zinfo);
//	tint =  new gridInterp(rx,ry,rz,Brt,xinfo,yinfo,zinfo);

//}


// Set the magnetic field scaling factor.

G4double Old_Magnetic_Field::setScale( G4double scale ) {
	return fScale = scale;
}

G4double Old_Magnetic_Field::setCurrent( G4double current ) {
  return fScale = current/6730.;
}

// Get the magnetic field scaling factor.

G4double Old_Magnetic_Field::getScale() {
	return fScale;
}

// Look-up 3D indices from 1D row-major index
void Old_Magnetic_Field::lookupInd(int j, int& ix, int& iy, int& iz)
{
	ix = j/fNz/fNy;
	iy = (j-ix*fNy*fNz)/fNz;
	iz = j-ix*fNy*fNz-iy*fNz;
	return;
}

void Old_Magnetic_Field::save(const char *filename)
{
   ofstream outfile (filename,ofstream::binary);

   if (!outfile.is_open())	{
      G4cerr << ERROR_MSG << "Could not open output file of magnetic field  >"
             << filename  << "<, exiting..." << NORMAL << G4endl;
      exit(1);
	}

   outfile.write((const char *)&fNdata,sizeof(fNdata));
   outfile.write((const char *)&fNx,sizeof(fNx));
   outfile.write((const char *)&fNy,sizeof(fNy));
   outfile.write((const char *)&fNz,sizeof(fNz));
   outfile.write((const char *)&fXmin,sizeof(fXmin));
   outfile.write((const char *)&fYmin,sizeof(fYmin));
   outfile.write((const char *)&fZmin,sizeof(fZmin));
   outfile.write((const char *)&fDx,sizeof(fDx));
   outfile.write((const char *)&fDy,sizeof(fDy));
   outfile.write((const char *)&fDz,sizeof(fDz));
   outfile.write((const char *)pfBx,sizeof(G4double)*fNxy*fNz);
   outfile.write((const char *)pfBy,sizeof(G4double)*fNxy*fNz);
   outfile.write((const char *)pfBz,sizeof(G4double)*fNxy*fNz);
   outfile.close();
}

