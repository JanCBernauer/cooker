#ifndef __COOKER_DETECTOR__
#define __COOKER_DETECTOR__

#include "G4String.hh"
#include "G4VUserDetectorConstruction.hh"

class Plugin;
class G4VPhysicalVolume;
class Magnetic_Field;

class cookerDetector : public G4VUserDetectorConstruction
{

 private:
  G4String gdmlfilename;
  std::string fieldfile;
  Plugin * plugin;
public:
  Magnetic_Field *pMF;
  G4String fieldHash, fieldFile;  // For output
  cookerDetector(char *,char *,Plugin *);
  void setCurrent(double);
  void initMagneticField();
  ~cookerDetector();
  
public:
  G4VPhysicalVolume* Construct();     //The queen of methods one has to implement
  
};

#endif
