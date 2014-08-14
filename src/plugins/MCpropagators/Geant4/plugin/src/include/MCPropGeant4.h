#ifndef __MCPROPGEANT4__
#define __MCPROPGEANT4__

#include "TObject.h"
#include "Plugin.h"
#include "TTree.h"
#include <iostream>
#include "TObjString.h"
#include "generatortree.h"
// #include "MCPropVis.h"

/*
// Geant4 headers for track info
#include "globals.hh"
#include "G4UserSteppingAction.hh"
#include "G4Step.hh"
#include "G4Track.hh"
*/

class cookerRM;
class cookerDetector;
class GeneratorEvent;
class MCPropVis;
class MCFieldIntegrator;
class SYMBFirstContactSteppingAction;

class MCPropGeant4:public Plugin
{
 private:
   std::string gdmlFileName;
	std::string fieldfile;
	cookerRM *runmanager;
	cookerDetector *cD;
	TObjString *fieldFile, *fieldHash;
	bool supBrem;
	bool fullBrem;
	bool stopz;
	GeneratorEvent * ge;

 public:
	MCPropGeant4(TTree *in, TTree *out,TFile *inf_, TFile * outf_, TObject *p);
	virtual ~MCPropGeant4();
	double *torCurrent;
	double fixedCurrent; // This is memory where torCurrent can point if we want a fixed current.
	//setup routines
	Long_t setGDMLFileName(char *);
	Long_t setFieldFile(char* cg);
	Long_t setVisMode(bool vis);
	bool vism;
	bool fint;

	// Geant4 User Action Object for Visualization
	MCPropVis * visStepAct;

	// Geant4 User Action Object for Field Integration
	MCFieldIntegrator * fieldIntAct;


	// process routines
	Long_t startup();
	Long_t startup_supBrem();
	Long_t startup_noBrem();
	Long_t startup_vis();
	Long_t process();
	Long_t process_vis();
	Long_t finalize();
	Long_t suppressBremPhotons(bool yn);
	Long_t suppressZeroWeightEvents(bool yn);
	Long_t noBrem(bool yn);
	Long_t setCurrent(double current); // An external way to set the current, if you don't want to use the SC
	virtual Long_t cmdline(char * cmd);

  ClassDef(MCPropGeant4,1);
};

#endif
