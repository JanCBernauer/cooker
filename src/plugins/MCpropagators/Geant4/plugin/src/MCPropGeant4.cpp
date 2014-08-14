#include <MCPropGeant4.h>

#include <iostream>
#include <cmath>
#include "cookerRM.h"
#include "cookerDetector.h"
#include "Physics.h"
#include "cookerGen.h"
#include "generatortree.h"
#include <vector>
#include <algorithm>
#include "G4UserSteppingAction.hh"
#include <string>
#include <stdlib.h>
#include "CLHEP/Units/PhysicalConstants.h"

// Visualization headers
#include "TGeoManager.h"
#include "TEveManager.h"
#include "TEveLine.h"
#include "TEvePointSet.h"

// Geant4 stepping action class to get info for track visualization
class MCPropVis : public G4UserSteppingAction
{
	public:

		MCPropVis();
		virtual ~MCPropVis();
		void reset();
		void UserSteppingAction(const G4Step* step);

	private:

		int lid, lind;
		std::vector<TEveLine*> tracks;
		std::vector<TEvePointSet*> hits;
		std::vector<int> ids;
};

class MCFieldIntegrator : public G4UserSteppingAction
{
	public:

		MCFieldIntegrator();
		virtual ~MCFieldIntegrator();
		void reset();
		void UserSteppingAction(const G4Step* step);

	private:
};



MCPropGeant4::MCPropGeant4(TTree *in, TTree *out,TFile *inf_, TFile * outf_, TObject *p):Plugin(in,out,inf_,outf_,p)
{
	torCurrent = NULL;

	// These are the default files in case the init file fails to set them
	gdmlFileName = "Geant4_survey2013.gdml";
	fieldfile = "cooker_v2.bin";

	// By default, visualization is off
	vism = false;

	// By default, don't do field integration
	fint = false;

	// By default, full bremsstrahlung
	fullBrem = true;
	supBrem  = false;

	// By default, propagate zero weight events
	stopz = false;
};

MCPropGeant4::~MCPropGeant4()
{
};

Long_t MCPropGeant4::setGDMLFileName(char * name)
{
  gdmlFileName = name;
  return 0;
}

Long_t MCPropGeant4::setFieldFile(char * name)
{
  fieldfile = name;
  return 0;
}

// Function to call in startup to signal that you want to visualize the
// propagation
Long_t MCPropGeant4::setVisMode(bool vis)
{
	vism = vis;
	return 0;
}

// Functions to set the bremsstrahlung mode
Long_t MCPropGeant4::noBrem(bool yn)
{
	fullBrem = !yn;
	supBrem  = !yn;

	return 0;
};

Long_t MCPropGeant4::suppressBremPhotons(bool yn)
{
	fullBrem = !yn;
	supBrem  = yn;

	return 0;
};

Long_t MCPropGeant4::suppressZeroWeightEvents(bool yn)
{
	stopz = yn;
	return 0;
};

// Startup functions for different bremsstrahlung modes (default: full brem)
Long_t MCPropGeant4::startup_supBrem()
{
	suppressBremPhotons(true);
	startup();
	return 0;
};

Long_t MCPropGeant4::startup_noBrem()
{
	noBrem(true);
	startup();
	return 0;
};


Long_t MCPropGeant4::startup()
{

	 // set the seed from global seed
	 int seed=* ( (int*) getMemoryObject("GlobalSeed") );
	 debug(1,"Propagation seed set to %i\n",seed);
	 CLHEP::HepRandom::setTheSeed(seed);
  
	// construct geant 4 and geometry
	debug(100,"Reading GDML to construct geometry\n");

	// Print the file names into the appropriate places
	char buffer[1024];
	snprintf(buffer,1000,"%s/.cooker/shared/gdml/%s",getenv("COOKERHOME"),gdmlFileName.c_str());
	char fieldbuf[1024];
	snprintf(fieldbuf,1000,"%s",fieldfile.c_str());

	//std::cout<<"\n\nField: "<<fieldfile<<"   Geo: "<<gdmlFileName<<"\n\n";

	runmanager=new cookerRM();



	cD = new cookerDetector(buffer,fieldbuf,this);

	runmanager->SetUserInitialization(cD);

	runmanager->SetUserInitialization( new Physics("",fullBrem,supBrem));

	// If visualization is requested, add the step action
	if (vism)
	{
		visStepAct = new MCPropVis();
		runmanager->SetUserAction(visStepAct);
	}

	// If field integration is requested, add the step action
	if (fint)
	{
		fieldIntAct = new MCFieldIntegrator();
		runmanager->SetUserAction(fieldIntAct);
	}


	GeneratorEvent * ge=NULL;
	getOutBranchObject("Generator",(TObject **) &ge);
	debug(0,"Out gen %p\n",ge);
	if (!ge)
	 {
		copyBranch("Generator");
		getBranchObject("Generator",(TObject **) &ge);
	 }
	if (!ge)
	 {
		debug(0,"Failed to find Generator Branch.\n");
		exit(-1);
	 }
	runmanager->SetUserAction(new cookerGen(ge));
	// runaction, eventaction?

	runmanager->Initialize();
	if (!runmanager->startRun())
	 {
		debug(0,"Error starting Geant4 run\n");
		exit(-1);
	 }

	// Need GD ROOT TObjStrings to put them in a tree
	fieldFile = new TObjString(cD->fieldFile.data());
	fieldHash = new TObjString(cD->fieldHash.data());

	// std::cout<<"\n\n"<<fieldFile->String()<<" "<<fieldHash->String()<<"\n\n";

	copyFileObject("ConfigData/trigger");

	return 0;
}

// Startup routine which can be called to flag visualization mode
Long_t MCPropGeant4::startup_vis()
{
	// Flag visualization mode
	vism = true;

	// Call normal startup
	startup();

	return 0;
};

Long_t MCPropGeant4::process()
{
	if (stopz)
		if (ge->weight.get_default()==0)
			return 0;
	cD->setCurrent(*torCurrent);
	runmanager->doEvent( in->GetReadEntry());
	return 0;
}

// Visualization process (separated for speed in normal mode)
Long_t MCPropGeant4::process_vis()
{

	// cout<<"EVENT\n";
	// Reset the visualization objects
	visStepAct->reset();

	// Normal process
	process();
};


Long_t MCPropGeant4::finalize()
{
	// Run Geant4 end processes
	runmanager->endRun();
	
	// Create output ROOT tree branches
	fieldFile->Write("BFieldFile");
	fieldHash->Write("BFieldHash");

	// Free memory
	delete runmanager;
	// if (vism) delete visStepAct;

	return 0;
}

Long_t MCPropGeant4::setCurrent(double current)
{
  std::cout << "MCPropGeant4: setting the magnet current to " << current << " A.\n";
  std::cout << "\tFor this propagation, the current is now fixed, and will not depend on the slow control.\n";
  fixedCurrent = current;
  torCurrent = &fixedCurrent; // if the current is set, then no need to get it from the slow ctrl.
  return 0;

	std::cout<<"\n\n";
}

Long_t MCPropGeant4::cmdline(char *cmd)
{
  //add cmdline hanling here

  return 0; // 0 = all ok
};


extern "C"{
Plugin *factory(TTree *in, TTree *out,TFile *inf_, TFile * outf_, TObject *p)
{
  return (Plugin *) new MCPropGeant4(in,out,inf_,outf_,p);
}
}

////////////////////////////////////////////////////////////////////////////////
//																										//
// Visualization class functions																//
//																										//
// Allow propagator tracks to be visualized in Event Display/visco framework  //
//																										//
// Added by B. Henderson, late August 2013												//
//																										//
////////////////////////////////////////////////////////////////////////////////

// Constructor/Destructor
MCPropVis::MCPropVis() {};

MCPropVis::~MCPropVis() {};

// This function, when registered with the Geant4 run manager should be executed
// on each execution step to provide position (or any other information) about
// the propagated tracks for visualization
void MCPropVis::UserSteppingAction(const G4Step* step)
{
	// Get the track ID for this step.
	int cid = step->GetTrack()->GetTrackID();

	// std::cout<<"Step! "<<cid<<" "<<step->GetTrack()->GetParticleDefinition()->GetParticleName()<<"\n";

	// Container index for the track information
	int ind = -1; // Intiallize to -1 to indicate index not yet found

	// If the track ID is the previous one save the search, else search through
	// the known tracks for the right one
	if (cid == lid) ind = lind;
	else
	{
		for (unsigned int j=0; j<ids.size(); j++)
		{
			if (ids[j] ==  cid)
			{
				ind = j;
				break;
			}
		}
	}

	// If the index hasn't been set after this, create a new track set, get the
	// particle type, and set the track's properties
	if (ind == -1)
	{
		ids.push_back(cid);
		tracks.push_back(new TEveLine);
		hits.push_back(new TEvePointSet);
		ind = ids.size()-1;  // Index is the end of the vector

		// Particle type
		G4String type = step->GetTrack()->GetParticleDefinition()->GetParticleName();

		// std::cout<<cid<<" "<<type<<"\n";

		// Set color based on particle type
		Color_t color;
		if (type.compare("proton") == 0)			color = kBlue;
	   else if (type.compare("e+") == 0)		color = kYellow-2;
	   else if (type.compare("e-") == 0)		color = kRed;
	   else if (type.compare("gamma") == 0)	color = kGreen;
		else if (type.compare("neutron") == 0) color = kCyan;
		else if (type.compare("mu+") == 0)     color = kGray+1;
		else if (type.compare("mu-") == 0)     color = kBlack;
		else if (type.compare("pi+") == 0)     color = kCyan+3;
		else if (type.compare("pi-") == 0)     color = kOrange;
	   else												color = kViolet;

		tracks[ind]->SetMainColor(color);
		tracks[ind]->SetLineColor(color);
		hits[ind]->SetMainColor(color);

		// Register the visualization set with the manager
		gEve->AddElement(tracks[ind]);
		gEve->AddElement(hits[ind]);		
	}

	// Save the current IDs for the next step
	lid = cid;
	lind = ind;

	// Add this step point to the appropriate track object
	G4ThreeVector poss = step->GetPreStepPoint()->GetPosition();
	tracks[ind]->SetNextPoint(poss.x()/10,poss.y()/10,poss.z()/10);
	gEve->AddElement(tracks[ind]);

	// Check to see if the step is in a sensitive detector.  If so, draw it as a
	// hit point
	if (step->GetPreStepPoint()->GetSensitiveDetector())
	{
		hits[ind]->SetNextPoint(poss.x()/10,poss.y()/10,poss.z()/10);
		gEve->AddElement(hits[ind]);
	}

	// std::cout<<(step->GetTrack()->GetParticleDefinition()->GetParticleName())<<"\n";
}

// Function to reset internal variables for a new event
void MCPropVis::reset()
{
	for (std::vector<TEveLine* >::iterator iter = tracks.begin(); iter != tracks.end(); iter++)
	{
		(*iter)->Reset(); // Remove points from geometry sets
		delete (*iter);
	}
	for (std::vector<TEvePointSet* >::iterator iter = hits.begin(); iter != hits.end(); iter++)
	{
		(*iter)->Reset(); // Remove points from geometry sets
		delete (*iter);
	}
	tracks.clear();	// Reset vectors of geometry sets
	hits.clear();
	ids.clear();		// List of track IDs cleared
	lid = -1;			// Last step indices
	lind = -1;

	// std::cout<<"\n\nRESET RESET RESET \n\n";
};

////////////////////////////////////////////////////////////////////////////////
//																										//
// Field integration class functions														//
//																										//
// Allow integration of field along propagated particle tracks					   //
//																										//
// Added by B. Henderson, late September 2013										 	//
//																										//
////////////////////////////////////////////////////////////////////////////////

// Constructor/Destructor
MCFieldIntegrator::MCFieldIntegrator() {};

MCFieldIntegrator::~MCFieldIntegrator() {};

// This function, when registered with the Geant4 run manager should be executed
// on each execution step to provide information about the field on each step
void MCFieldIntegrator::UserSteppingAction(const G4Step* step)
{
	// Get the track ID for this step, we only want the primary
	int cid = step->GetTrack()->GetTrackID();

}

// Function to reset internal variables for a new event
void MCFieldIntegrator::reset()
{

};





ClassImp(MCPropGeant4);
