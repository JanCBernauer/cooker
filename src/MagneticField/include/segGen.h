#ifndef __SEGGEN_H__
#define __SEGGEN_H__

#include "CLHEP/Vector/ThreeVector.h"

double * fillVector(double * memBlock, double x, double y, double z);
int segGen(double * memBlock, int nSegsBigArc, int nSegsSmallArc);
int stepGen(double * memBlock, int nSegsBigArc, int nSegsSmallArc, double uStrLen, double dStrLen, double dStepRad);
int restGen(double * memBlock, int nSegsBigArc, int nSegsSmallArc);
double * addArc(double * memBlock, CLHEP::Hep3Vector c, CLHEP::Hep3Vector s, CLHEP::Hep3Vector n, double a, int nSegs);

#endif
