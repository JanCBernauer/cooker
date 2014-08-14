#ifndef __BASE_SD__
#define __BASE_SD__

#include "G4VSensitiveDetector.hh"
#include <vector>
#include <bitset>
class Plugin;
class MCDetectorData;

class base_SD:public G4VSensitiveDetector
{
 protected:
  std::string treeName;
  MCDetectorData *data;
  Plugin * plugin;

  std::bitset<2000> transformValid;
  std::vector<G4AffineTransform> WorldtoLocal;
  std::vector<G4AffineTransform> LocaltoWorld;

 public:
  base_SD( G4String name, std::string treename, Plugin *plg);
  ~base_SD();
  virtual void  Initialize(G4HCofThisEvent*);
  virtual G4bool ProcessHits( G4Step * step, G4TouchableHistory * /*ROhist*/ );
};

#endif
