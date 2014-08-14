#ifndef MAGNETIC_FIELD_H
#define MAGNETIC_FIELD_H

// Hide Geant4 headers from CINT
#ifndef __MAKECINT__
#include "G4MagneticField.hh"
#else
class G4MagneticField;
class G4double;
class G4int;
#endif

// Include the STL header files referenced here.
#include <vector>

// Spline interpolation headers
class gridInterp;
class ffInterpolator;
//#include <TSpline.h>
//#include "spline_interp.h"

// ROOT checksum code
#include "TMD5.h"

class Magnetic_Field:public G4MagneticField
{
 public:
	// Main field query function
  TMD5 * fieldCS; // Checksum for storage in the ROOT tree to see which grid was used.
  virtual void GetFieldValue( const G4double Point[4], G4double * Bfield ) const =0;
  virtual double setCurrent( double)=0;
  virtual double getCurrent()=0;
 Magnetic_Field():G4MagneticField(){};


};


class Spline_Field : public Magnetic_Field
{
private:
	double magnetCurrent;         //!<Magnet Current (A) is used as a scale factor

	// Data stored in the header of the main field grid
	G4int coords;		 //!< Grid coordinate type
	G4int fNdata;            //!< Number of grid points.
	G4int fNx;               //!< Number of grid steps in X direction.
	G4int fNy;               //!< Number of grid steps in Y direction.
	G4int fNz;               //!< Number of grid steps in Z direction.
	G4int fNxy;              //!< Accelerate index lookup.
	G4double fXmin;          //!< Starting X coordinate for grid.
	G4double fYmin;          //!< Starting Y coordinate for grid.
	G4double fZmin;          //!< Starting Z coordinate for grid.
	G4double fDx;            //!< Step size in X direction.
	G4double fDy;            //!< Step size in Y direction.
	G4double fDz;            //!< Step size in Z direction.

	// Data for the auxiliary SYMB region grid
	// All header values named with same convention as above for the main grid
	// with f->s (and not using silly G4 variables)
	float * sbx, * sby, * sbz; // Field values
	int sNdata;
	int sNx;
	int sNy;
	int sNz;
	int sNxz;
	double sXmin;
	double sYmin;
	double sZmin;
	double sDx, isDx;
	double sDy, isDy;
	double sDz, isDz;

	// New fast spline stuff
	std::vector<int> info;
	ffInterpolator *bint;

	// Flag to say whether a special SYMB field region is in effect
	bool symb;

	// Reading the data file
	void read(const char * filename); // Read the grid file in the spline grid format

public:

	// Constructor/destructor
	Spline_Field( const char * filename, double current);
	~Spline_Field();


	// Main field query function
	void GetFieldValue( const G4double Point[4], G4double * Bfield ) const; // Point (x,y,z,t), Bfield (Bx,By,Bz,0,0,0)
	void symbInt(double qx, double qy, double qz, double& bx, double& by, double& bz) const;

	// Get/Set for internal private values
	double setCurrent( double current = 5000.0 );
	double getCurrent();
	bool symbField();
};

/**************************************************************************************

Old Magnetic Field class is for reading grid files in the BLAST format. This is only
to be used for testing purposes, and can otherwise be safely ignored.

**************************************************************************************/

class Old_Magnetic_Field : public Magnetic_Field {

	G4double *pfBx, *pfBy, *pfBz; //!< Arrays to hold the magnetic field values.

	G4int fNdata;            //!< Number of grid points.
	G4int fNx;               //!< Number of grid steps in X direction.
	G4int fNy;               //!< Number of grid steps in Y direction.
	G4int fNz;               //!< Number of grid steps in Z direction.
	G4int fNxy;              //!< Accelerate index lookup.
	G4int coords;				 //!< Grid coordinate type
	G4double fXmin;          //!< Starting X coordinate for grid.
	G4double fYmin;          //!< Starting Y coordinate for grid.
	G4double fZmin;          //!< Starting Z coordinate for grid.

	G4double fDx;            //!< Step size in X direction.
	G4double fDy;            //!< Step size in Y direction.
	G4double fDz;            //!< Step size in Z direction.

	G4double fiDx;            //!< inv. Step size in X direction.
	G4double fiDy;            //!< inv. Step size in Y direction.
	G4double fiDz;            //!< inv. Step size in Z direction.

	G4double fScale;         //!< Scale factor for calculating the field.

	// Old spline method
	// Interpolation objects and utility functions
	// double *rx, *ry, *rz, *sphi, *cphi;	// Cartesian coordinates at each point, plus cylindrical angle
	// double *Brx, *Bry, *Brz;				// Real-only double arrays for Cartesian field interpolation
	// double *Brr, *Brt;						// And for cylindrical components
	void lookupInd(int j, int& ix, int& iy, int& iz); // Index look-up utility
	// gridInterp *xint, *yint, *zint, *rint, *tint;  // 3D interpolation class objects
	// void setupSpline();									  // Function to hide-away the spline setup*/

	// New fast spline method stuff
	bool ffMode;
	std::vector<int> info;
	ffInterpolator *bxint, *byint, *bzint;
	// float *abx, *abxdx, *abxdy, *abxdz, *abxdxy, *abxdxz, *abxdyz, *abxdxyz,
	//		*aby, *abydx, *abydy, *abydz, *abydxy, *abydxz, *abydyz, *abydxyz,
	//		*abz, *abzdx, *abzdy, *abzdz, *abzdxy, *abzdxz, *abzdyz, *abzdxyz;

	void readTextFile(const char * filename); // Two routines for reading in the field grid
	void readBinFile(const char * filename);
	void readDerivBinFile(const char * filename); // New spline grid format

public:

	Old_Magnetic_Field( const char * filename, G4double scale);
	~Old_Magnetic_Field();

	TMD5 * fieldCS; // Checksum for storage in the ROOT tree to see which grid was used.

	void GetFieldValue( const G4double Point[4], G4double * Bfield ) const; // Point (x,y,z,t), Bfield (Bx,By,Bz,0,0,0)
	G4double setScale( G4double scale = 1.0 );
	G4double setCurrent( G4double current );
	G4double getScale();

	void save(const char *filename);
};


/**************************************************************************************

NoInterp_Magnetic_Field class calculates the field directly from a set of current
segments. There is no interpolation between grid points done in the computation.

**************************************************************************************/

class NoInterp_Magnetic_Field : public Magnetic_Field
{
 private:
  G4double current;
  void initialize();

 public:
  NoInterp_Magnetic_Field(G4double cur);
  ~NoInterp_Magnetic_Field();
  
  void GetFieldValue( const G4double Point[4], G4double * Bfield ) const; // Point (x,y,z,t), Bfield (Bx,By,Bz,0,0,0)
  G4double setCurrent( G4double i );
  G4double getCurrent();
};

#endif
