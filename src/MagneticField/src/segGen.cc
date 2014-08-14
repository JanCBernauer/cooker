#include "segGen.h"

#include <cmath>

#define NROWS (2)
#define NCOND (13)

#define COND_DIST_Z (20.379182) // Distance between conductor and midplane, in z
#define DIST_TO_EDGE (25.3746) // Distance from edge of epoxy to first conductor, in y
#define COND_DIST_Y (40.2209) // Distance between conductors in y
#define NARROW_END_RAD (255.) // Inner edge radius of the narrow end arc
#define WIDE_END_RAD (430.) // Inner edge radius of the wide end arc
#define LONG_STRAIGHT_LENGTH (2574.8) // Length of the long straight section, in x
#define DOWN_STRAIGHT_LENGTH (447.5) // Length of the downstream straight section, in x
#define UP_STRAIGHT_LENGTH (1127.3) // Length of the upstream straight section, in x
#define DSTEPRAD (531.9) // Inner radius of the downstream curve in the step
//#define USTEPRAD (538.0) // Inner radius of the upstream curve in the step

double * fillVector(double * memBlock, double x, double y, double z)
{
  memBlock[0] = x; memBlock[1] = y; memBlock[2] = z;
  return memBlock+3;
}

int segGen(double * memBlock, int nSegsBigArc, int nSegsSmallArc)
{
  const double fullWidth = 2. * DIST_TO_EDGE + NCOND * COND_DIST_Y;
  const double stepSlope = (2.* (WIDE_END_RAD - NARROW_END_RAD) / (LONG_STRAIGHT_LENGTH - UP_STRAIGHT_LENGTH - DOWN_STRAIGHT_LENGTH));
  const double stepAngle = asin(2.*stepSlope / (stepSlope*stepSlope + 1.));
  //  const double stepRad = 0.5*((LONG_STRAIGHT_LENGTH - UP_STRAIGHT_LENGTH - DOWN_STRAIGHT_LENGTH)/sin(stepAngle) - fullWidth);
  const double dStepRad = DSTEPRAD;
  const double uStepRad = (LONG_STRAIGHT_LENGTH - UP_STRAIGHT_LENGTH - DOWN_STRAIGHT_LENGTH)/sin(stepAngle) - fullWidth - dStepRad;

  int counter = 0;

  CLHEP::Hep3Vector center,start,norm;

  for (int r=0 ; r<NROWS ;r++)
    {
      // Define the z value for the row
      double z = (r==0)? -COND_DIST_Z : COND_DIST_Z;

      for (int c=0 ; c<NCOND ;c++)
	{
	  double widthFromInnerEdge = DIST_TO_EDGE + c*COND_DIST_Y;

	  // We'll follow the direction of the current, starting with the long straight section
	  // and finishing with the 180 degree arc at the narrow end

	  // Long straight section
	  memBlock = fillVector(memBlock,0.,-0.5*fullWidth + widthFromInnerEdge,z); // center
	  memBlock = fillVector(memBlock,LONG_STRAIGHT_LENGTH,0.,0.); // length
	  counter++;

	  // Wide 180-degree arc
	  center.set(0.5*LONG_STRAIGHT_LENGTH,-0.5*fullWidth-WIDE_END_RAD,z);
	  start.set(0.,WIDE_END_RAD + widthFromInnerEdge,0.);
	  norm.set(0.,0.,-1.);
	  memBlock = addArc(memBlock,center, start,norm,M_PI,nSegsBigArc);
	  counter += nSegsBigArc;

	  // Straight section, downstream of step
	  memBlock = fillVector(memBlock,0.5*(LONG_STRAIGHT_LENGTH - DOWN_STRAIGHT_LENGTH),-0.5*fullWidth-2.*WIDE_END_RAD-widthFromInnerEdge,z);
	  memBlock = fillVector(memBlock,-DOWN_STRAIGHT_LENGTH,0.,0.);
	  counter++;

	  // Downstream end of step
	  center.set(0.5*LONG_STRAIGHT_LENGTH - DOWN_STRAIGHT_LENGTH,-0.5*fullWidth-2.*WIDE_END_RAD+dStepRad,z);
	  start.set(0.,-dStepRad-widthFromInnerEdge,0.);
	  norm.set(0.,0.,-1.);
	  memBlock = addArc(memBlock,center,start,norm,stepAngle,nSegsSmallArc);
	  counter += nSegsSmallArc;

	  // Upstream end of step
	  center.set(-0.5*LONG_STRAIGHT_LENGTH + UP_STRAIGHT_LENGTH,-1.5*fullWidth - 2.*NARROW_END_RAD - uStepRad,z);
	  double upStepRad = uStepRad + fullWidth - widthFromInnerEdge;
	  start.set(upStepRad*sin(stepAngle),upStepRad*cos(stepAngle),0.);
	  norm.set(0.,0.,1.);
	  memBlock = addArc(memBlock,center,start,norm,stepAngle,nSegsSmallArc);
	  counter += nSegsSmallArc;

	  // Straight section, upstream of step
	  memBlock = fillVector(memBlock,0.5*(UP_STRAIGHT_LENGTH - LONG_STRAIGHT_LENGTH),-0.5*fullWidth - 2.*NARROW_END_RAD - widthFromInnerEdge,z);
	  memBlock = fillVector(memBlock,-UP_STRAIGHT_LENGTH,0.,0.);
	  counter++;

	  // Narrow 180 degree arc
	  center = CLHEP::Hep3Vector(-0.5*LONG_STRAIGHT_LENGTH,-0.5*fullWidth-NARROW_END_RAD,z);
	  start = CLHEP::Hep3Vector(0.,-NARROW_END_RAD - widthFromInnerEdge,0.);
	  norm = CLHEP::Hep3Vector(0.,0.,-1.);
	  memBlock = addArc(memBlock,center,start,norm,M_PI,nSegsBigArc);
	  counter += nSegsBigArc;
	}
    }
  return counter;
}

int stepGen(double * memBlock, int nSegsBigArc, int nSegsSmallArc, double uStrLen, double dStrLen, double dStepRad)
{
  const double fullWidth = 2. * DIST_TO_EDGE + NCOND * COND_DIST_Y;
  const double stepSlope = (2.* (WIDE_END_RAD - NARROW_END_RAD) / (LONG_STRAIGHT_LENGTH - uStrLen - dStrLen));
  const double stepAngle = asin(2.*stepSlope / (stepSlope*stepSlope + 1.));
  const double uStepRad = (LONG_STRAIGHT_LENGTH - uStrLen - dStrLen)/sin(stepAngle) - fullWidth - dStepRad;

  int counter = 0;

  CLHEP::Hep3Vector center,start,norm;

  for (int r=0 ; r<NROWS ;r++)
    {
      // Define the z value for the row
      double z = (r==0)? -COND_DIST_Z : COND_DIST_Z;

      for (int c=0 ; c<NCOND ;c++)
	{
	  double widthFromInnerEdge = DIST_TO_EDGE + c*COND_DIST_Y;

	  // We'll follow the direction of the current

	  // Straight section, downstream of step
	  memBlock = fillVector(memBlock,0.5*(LONG_STRAIGHT_LENGTH - dStrLen),-0.5*fullWidth-2.*WIDE_END_RAD-widthFromInnerEdge,z);
	  memBlock = fillVector(memBlock,-dStrLen,0.,0.);
	  counter++;

	  // Downstream end of step
	  center.set(0.5*LONG_STRAIGHT_LENGTH - dStrLen,-0.5*fullWidth-2.*WIDE_END_RAD+dStepRad,z);
	  start.set(0.,-dStepRad-widthFromInnerEdge,0.);
	  norm.set(0.,0.,-1.);
	  memBlock = addArc(memBlock,center,start,norm,stepAngle,nSegsSmallArc);
	  counter += nSegsSmallArc;

	  // Upstream end of step
	  center.set(-0.5*LONG_STRAIGHT_LENGTH + uStrLen,-1.5*fullWidth - 2.*NARROW_END_RAD - uStepRad,z);
	  double upStepRad = uStepRad + fullWidth - widthFromInnerEdge;
	  start.set(upStepRad*sin(stepAngle),upStepRad*cos(stepAngle),0.);
	  norm.set(0.,0.,1.);
	  memBlock = addArc(memBlock,center,start,norm,stepAngle,nSegsSmallArc);
	  counter += nSegsSmallArc;

	  // Straight section, upstream of step
	  memBlock = fillVector(memBlock,0.5*(uStrLen - LONG_STRAIGHT_LENGTH),-0.5*fullWidth - 2.*NARROW_END_RAD - widthFromInnerEdge,z);
	  memBlock = fillVector(memBlock,-uStrLen,0.,0.);
	  counter++;
	}
    }
  return counter;
}

int restGen(double * memBlock, int nSegsBigArc, int nSegsSmallArc)
{
  const double fullWidth = 2. * DIST_TO_EDGE + NCOND * COND_DIST_Y;
  int counter = 0;

  CLHEP::Hep3Vector center,start,norm;

  for (int r=0 ; r<NROWS ;r++)
    {
      // Define the z value for the row
      double z = (r==0)? -COND_DIST_Z : COND_DIST_Z;

      for (int c=0 ; c<NCOND ;c++)
	{
	  double widthFromInnerEdge = DIST_TO_EDGE + c*COND_DIST_Y;

	  // We'll follow the direction of the current, starting with the long straight section
	  // and finishing with the 180 degree arc at the narrow end

	  // Long straight section
	  memBlock = fillVector(memBlock,0.,-0.5*fullWidth + widthFromInnerEdge,z); // center
	  memBlock = fillVector(memBlock,LONG_STRAIGHT_LENGTH,0.,0.); // length
	  counter++;

	  // Wide 180-degree arc
	  center.set(0.5*LONG_STRAIGHT_LENGTH,-0.5*fullWidth-WIDE_END_RAD,z);
	  start.set(0.,WIDE_END_RAD + widthFromInnerEdge,0.);
	  norm.set(0.,0.,-1.);
	  memBlock = addArc(memBlock,center, start,norm,M_PI,nSegsBigArc);
	  counter += nSegsBigArc;

	  // Narrow 180 degree arc
	  center = CLHEP::Hep3Vector(-0.5*LONG_STRAIGHT_LENGTH,-0.5*fullWidth-NARROW_END_RAD,z);
	  start = CLHEP::Hep3Vector(0.,-NARROW_END_RAD - widthFromInnerEdge,0.);
	  norm = CLHEP::Hep3Vector(0.,0.,-1.);
	  memBlock = addArc(memBlock,center,start,norm,M_PI,nSegsBigArc);
	  counter += nSegsBigArc;
	}
    }
  return counter;
}

double * addArc(double * memBlock, CLHEP::Hep3Vector c, CLHEP::Hep3Vector s, CLHEP::Hep3Vector n, double a, int nSegs)
{
  CLHEP::Hep3Vector sDir = s.unit();
  CLHEP::Hep3Vector normalVec = (n - (s.dot(n) * sDir)).unit(); // Vector normal to the plane of the arc
  CLHEP::Hep3Vector curDir = normalVec.cross(sDir);
  double radius = s.mag(); // Actual radius of the arc
  double rPrime = radius * (sqrt( 1./((nSegs-2.)*(nSegs-2.)) + a / ((nSegs-2.)*sin(a/nSegs))) - 1./(nSegs-2.)); // Radius of non-end points for equal area
  double angleStep = a / ((double)nSegs); // Angle of each segment

  CLHEP::Hep3Vector centerPoint, length;

  double angle = 0; // angle of current segment;
  CLHEP::Hep3Vector startPoint = c + s;
  CLHEP::Hep3Vector endPoint;
  for (int i = 0 ; i<(nSegs-1) ; i++)
    {
      angle += angleStep;
      endPoint = c + rPrime*(sDir * cos(angle) + curDir * sin(angle));
      centerPoint = 0.5 * (startPoint + endPoint);
      length = endPoint - startPoint;
      memBlock = fillVector(memBlock,centerPoint.x(),centerPoint.y(),centerPoint.z());
      memBlock = fillVector(memBlock,length.x(),length.y(),length.z());

      // Make the new startPoint equal to the old endPoint
      startPoint = endPoint;
    }

  // Now we have to do the last segment
  angle += angleStep;
  endPoint = c + radius * (sDir * cos(angle) + curDir * sin(angle));
  centerPoint = 0.5 * (startPoint + endPoint);
  length = endPoint - startPoint;
  memBlock = fillVector(memBlock,centerPoint.x(),centerPoint.y(),centerPoint.z());
  memBlock = fillVector(memBlock,length.x(),length.y(),length.z());

  return memBlock;
}
