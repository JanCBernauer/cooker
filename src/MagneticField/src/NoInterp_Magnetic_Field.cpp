#include "Magnetic_Field.h"
#include "segGen.h"
#include "globals.hh"
#include "G4ios.hh"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/Rotation.h"
#include <cmath>
#include <iostream>
#include <cstdlib>

// Stuff for field calculations
//     ** Defines **
//#define NSEGSBIGARC (180)
//#define NSEGSSMALLARC (40)
#define NSEGSBIGARC (18)
#define NSEGSSMALLARC (4)
#define NCONDS (26)
const int nSegs = NCONDS * (3 + 2 * NSEGSBIGARC + 2 * NSEGSSMALLARC);
double segments[6*nSegs];
#define NCOILS 8
//     ** Coil placement **
CLHEP::Hep3Vector translations[NCOILS];
CLHEP::HepRotation rotations[NCOILS];
double shortScale;
double longScale;
int nRestSegs;
//     ** Parameters **
const int nparams = 35;
double params[nparams];
//     ** Nominal Placement **
double xRotations[NCOILS] = {90.,90.,-90.,-90.,-90.,-90.,90.,90.};
double yRotations[NCOILS] = {22.5,67.5,67.5,22.5,-22.5,-67.5,-67.5,-22.5};
double zRotations[NCOILS] = {90.,90.,-90.,-90.,-90.,-90.,90.,90.};
const double coilAngle = 2. * M_PI / ((double)NCOILS);
const double coilRho = 1810.1;
const double coilZ = 651.1;
#define DSTRLEN (447.5)
#define USTRLEN (1127.3)
#define DSTEPRAD (531.9)
double dStrLen = DSTRLEN;
double uStrLen = USTRLEN;
double dStepRad = DSTEPRAD;

#define MUOVERFOURPI (1.E-4) // in units of tesla * mm / ampere

void magFieldFromSegment(double * segment, double * pos, double * result)
{
  CLHEP::Hep3Vector center(segment[0],segment[1],segment[2]);
  CLHEP::Hep3Vector length(segment[3],segment[4],segment[5]);
  CLHEP::Hep3Vector position(pos[0],pos[1],pos[2]);
  CLHEP::Hep3Vector dist = center - position;
  double L = length.mag();
  double A = length.dot(dist) / L;
  double BSq = dist.mag2() - A*A;
  double APlusHalfL = (A + 0.5 * L);
  double AMinusHalfL = (A - 0.5 * L);
  double factor = APlusHalfL/sqrt(BSq + APlusHalfL*APlusHalfL) - AMinusHalfL/sqrt(BSq + AMinusHalfL*AMinusHalfL);
  
  CLHEP::Hep3Vector field = ((MUOVERFOURPI * factor) / (BSq * L )) * dist.cross(length);
  result[0] = field.x();
  result[1] = field.y();
  result[2] = field.z();
}

void transformSegmentWithScale(const CLHEP::Hep3Vector &tra, const CLHEP::HepRotation &rot, double longScale, double shortScale, double * local, double * transformed)
{
  CLHEP::Hep3Vector center(longScale*local[0],shortScale*local[1],local[2]);
  CLHEP::Hep3Vector length(longScale*local[3],shortScale*local[4],local[5]);
  
  center = center.transform(rot) + tra;
  length = length.transform(rot);

  transformed[0] = center.x(); transformed[1] = center.y(); transformed[2] = center.z();
  transformed[3] = length.x(); transformed[4] = length.y(); transformed[5] = length.z();
}

void calcFieldFromCoil(int coilNo, double * pos, double * result)
{
  // We have to re-zero the field;
  result[0] = 0.; result[1]=0.; result[2]=0.;

  for (int s=0; s < nSegs ; s++)
    {
      double transformed[6];
      transformSegmentWithScale(translations[coilNo],rotations[coilNo],longScale,shortScale,segments+6*s,transformed);

      double temp[3];
      magFieldFromSegment(transformed, pos, temp); // For 1A current.

      for (int i=0 ; i<3 ;i++)
	result[i]+=temp[i];
    }
}

void updateTrans(const double * p)
{
  longScale = p[0];
  shortScale = p[1];
  double x0 = p[2];
  double y0 = p[3];
  double z0 = p[4];
  double rX = p[5];
  double rY = p[6];
  double rZ = p[7];
  double rCoil = p[8];
  double dRho = p[9];
  const double * pCoil = p+10;

  int tvcoil = 0;
  int ocoil = 0;
  for (int c=0 ; c<NCOILS ;c++)
    {
      double x,y,z,rho;
      double phi = coilAngle * (c+0.5);
      z = coilZ + z0;
      rotations[c] = CLHEP::HepRotation(0.,0.,0.);
      // Make a distinction between tracking volume coils and others
      if ((c==1)||(c==2)||(c==5)||(c==6))
	{
	  rho = coilRho + dRho;
	  rotations[c].rotateZ(-rCoil * M_PI / 180.);
	  ocoil++;
	}
      else
	{
	  rho = coilRho + pCoil[6*tvcoil+0];
	  phi += pCoil[6*tvcoil+1]; // needs to be converted from degrees
	  z += pCoil[6*tvcoil+2];
	  rotations[c].rotateZ(-pCoil[6*tvcoil+5]*M_PI/180.);
	  rotations[c].rotateY(-pCoil[6*tvcoil+4]*M_PI/180.);
	  rotations[c].rotateX(-pCoil[6*tvcoil+3]*M_PI/180.);
	  tvcoil++;
	}

      x = rho * cos(phi) + x0;
      y = rho * sin(phi) + y0;

      translations[c] = CLHEP::Hep3Vector(x,y,z);
      rotations[c].rotateZ(-zRotations[c] * M_PI / 180.);
      rotations[c].rotateY(-yRotations[c] * M_PI / 180.);
      rotations[c].rotateX(-xRotations[c] * M_PI / 180.);
      rotations[c].rotateZ(-rZ * M_PI / 180.);
      rotations[c].rotateY(-rY * M_PI / 180.);
      rotations[c].rotateX(-rX * M_PI / 180.);
    }
}

NoInterp_Magnetic_Field::NoInterp_Magnetic_Field(G4double cur)
{
  current = cur;
  initialize();
}


NoInterp_Magnetic_Field::~NoInterp_Magnetic_Field()
{
}
  
void NoInterp_Magnetic_Field::initialize()
{
  // Hardcode the parameters for the coil placement
  params[0]=5006.84;
  params[1]=0.985822;
  params[2]=0.9982;
  params[3]=3.35674;
  params[4]=-21.7003;
  params[5]=4.63252;
  params[6]=1.36807;
  params[7]=0.016498;
  params[8]=-0.163397;
  params[9]=-0.0495801;
  params[10]=12.3495;
  params[11]=26.5842;
  params[12]=0.00990677;
  params[13]=8.21411;
  params[14]=-0.0402859;
  params[15]=-0.938968;
  params[16]=0.402455;
  params[17]=24.6394;
  params[18]=-0.00621628;
  params[19]=1.42923;
  params[20]=-0.0654962;
  params[21]=1.1413;
  params[22]=0.634673;
  params[23]=-4.96187;
  params[24]=-0.00563976;
  params[25]=0.331756;
  params[26]=0.120429;
  params[27]=1.10007;
  params[28]=-0.845846;
  params[29]=-1.49355;
  params[30]=0.00201339;
  params[31]=22.7439;
  params[32]=0.546912;
  params[33]=-0.891447;
  params[34]=-0.491546;

  fieldCS = new TMD5();
  fieldCS->Update((unsigned char *)params,sizeof(double)*34);
  fieldCS->Final();

  // Define the coil positions and rotations.
  updateTrans(params+1);

  // Generate the segments
  nRestSegs = restGen(segments,NSEGSBIGARC,NSEGSSMALLARC);
  int nStepSegs = stepGen(segments + 6*nRestSegs,NSEGSBIGARC,NSEGSSMALLARC,uStrLen,dStrLen,dStepRad);
  if ((nStepSegs+nRestSegs)!=nSegs)
    {
      std::cerr << "WRONG NUMBER OF SEGMENTS!\n";
      exit (-1);
    }
}

void NoInterp_Magnetic_Field::GetFieldValue( const G4double Point[4], G4double * Bfield ) const // Point (x,y,z,t), Bfield (Bx,By,Bz,0,0,0)
{
  // Rezero the field;
  Bfield[0]=0.;
  Bfield[1]=0.;
  Bfield[2]=0.;
  Bfield[3]=0.;
  Bfield[4]=0.;
  Bfield[5]=0.;

  double pos[3]={Point[0],Point[1],Point[2]};

  // Now loop through the coils and recalculate the field
  for (int c=0 ; c<NCOILS ; c++)
    {
      double res[3];
      
      calcFieldFromCoil(c,pos,res);
      for (int d=0 ; d<3 ; d++)
	Bfield[d]+=res[d];
    }
  
  for (int d=0 ; d<3 ; d++)
    Bfield[d]*= (current * tesla * (params[0]/5000.)) ;
 }

G4double NoInterp_Magnetic_Field::setCurrent( G4double i )
{
  current = i;
}

G4double NoInterp_Magnetic_Field::getCurrent()
{
  return current;
}
