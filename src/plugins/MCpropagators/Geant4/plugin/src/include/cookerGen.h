#ifndef __COOKER_GEN_
#define __COOKER_GEN_

#include "G4VUserPrimaryGeneratorAction.hh"
#include <map>
#include <string>
class GeneratorEvent;
class G4ParticleDefinition;

class cookerGen:public G4VUserPrimaryGeneratorAction
{
 private:
  GeneratorEvent *genEvent;
  std::map<std::string,G4ParticleDefinition*> particleDefs;
 public:
  cookerGen(GeneratorEvent *ge);
  void GeneratePrimaries (G4Event *anEvent);
};


#endif
