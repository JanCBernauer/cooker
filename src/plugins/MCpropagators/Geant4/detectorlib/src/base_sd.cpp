//! \file
//!
//! Source file for Basic sensitive detector
//!
//! The base_sd class defines the member functions for processing the hit
//! information whenever a hit is simulated in the detector.  These
//! routines initialise the output tree, process hits, and perform
//! the end of event action which does nothing.
//!
//! \author JCB
//! \version 1.0
//! \date 2013-02-15
//!
//! \ingroup detector

// *+****1****+****2****+****3****+****4****+****5****+****6****+****7****+****

#include "base_SD.h"

#include "Plugin.h"
#include "mctree.h"


base_SD::base_SD( G4String name, std::string treename, Plugin *plg) : G4VSensitiveDetector( name ),plugin(plg),data(NULL),treeName(treename)
 {

   for (int i=0;i<2000;i++)
     transformValid[i]=false;
   
   data=new MCDetectorData();
   plugin->makeBranch(treeName.c_str(),(TObject **) &data);
   
}

base_SD::~base_SD() {

}

void base_SD::Initialize(G4HCofThisEvent*)
{
  data->hits.clear();
}

// Process hits.
G4bool base_SD::ProcessHits( G4Step * step, G4TouchableHistory * /*ROhist*/ ) {

  MCDetectorHit hit;

  hit.Edep=step->GetTotalEnergyDeposit();
  
  G4StepPoint * prestep = step->GetPreStepPoint();
  G4StepPoint * poststep = step->GetPostStepPoint();

  G4TouchableHandle touchable = prestep->GetTouchableHandle();

  hit.id=touchable->GetCopyNumber();
  if (hit.id>=2000)
    {
      plugin->debug(0,"Hit id is too large... bailing out \n");
      exit(-1);
    }

  G4Track * track = step->GetTrack();

  hit.time = prestep->GetGlobalTime();
  hit.track = track->GetTrackID();

   // Get the true world and local coordinate positions.
  
   G4ThreeVector trueworld =
      ( prestep->GetPosition() + poststep->GetPosition() ) / 2.0;

   // If this first time store transformation and inverse.

   if( !transformValid[hit.id] ) {
     if (WorldtoLocal.size()<hit.id+1) //too small
       {
	 WorldtoLocal.resize(hit.id+1);
	 LocaltoWorld.resize(hit.id+1);
       }
      WorldtoLocal[hit.id] = touchable->GetHistory()->GetTopTransform();
      LocaltoWorld[hit.id] = WorldtoLocal[hit.id].Inverse();
      transformValid[hit.id] = true;
   }

   G4ThreeVector Tmp = WorldtoLocal[hit.id].TransformPoint(trueworld);

   hit.world=TVector3(trueworld.x(),trueworld.y(),trueworld.z());
   hit.local=TVector3(Tmp.x(),Tmp.y(),Tmp.z());

   // find average momentum

   G4ThreeVector mom=0.5*(prestep->GetMomentum()+poststep->GetMomentum());
   hit.fourmomentum.SetXYZM(mom.x()/MeV,mom.y()/MeV,mom.z()/MeV,prestep->GetMass()/MeV);
   mom=WorldtoLocal[hit.id].TransformAxis(mom);
   hit.localmomentum=TVector3(mom.x(),mom.y(),mom.z());
   data->hits.push_back(hit);
   return true;
}



