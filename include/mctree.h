#ifndef __MCTREE_H_
#define __MCTREE_H_
#include "cookerrawtree.h"
#include <vector>
#include "TVector3.h"
#include "TLorentzVector.h"

class MCDetectorHit:public CRTBase
{
 public:
  unsigned int id;
  unsigned int track;
  double time;
  double Edep;
  TVector3 world;
  TVector3 local;
  TLorentzVector fourmomentum;
  TVector3  localmomentum;

  MCDetectorHit();
  ~MCDetectorHit();
  ClassDef(MCDetectorHit,1);
};

class MCDetectorData:public CRTBase
{
 public:
  std::vector<MCDetectorHit> hits;

  MCDetectorData();
  ~MCDetectorData();
  ClassDef(MCDetectorData,1);
};


#endif
