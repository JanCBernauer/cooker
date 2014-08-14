#ifndef _MCTRACKER_H__
#define _MCTRACKER_H__

#include "G4String.hh"
#include "G4ThreeVector.hh"
#include "G4AffineTransform.hh"
#include "G4ErrorMatrix.hh"
#include "G4ErrorPropagatorData.hh"
#include "G4ErrorSurfaceTrajState.hh"

#include <vector>
#include <map>


class G4ErrorTarget;
class G4ErrorPropagatorManager;
class G4MagneticField;
class TrackDetectorConstruction;
class G4ErrorFreeTrajState; 
class G4VPhysicalVolume;
class G4ErrorTrackLengthTarget;

class MCDetector
{
 public:
  G4String name;
  G4AffineTransform wtol;
  G4AffineTransform ltow;
  G4ThreeVector p0;
  G4ThreeVector pV;
  G4ThreeVector pW;
  G4ErrorTarget *target;
  G4VPhysicalVolume *physvol;
};


//typedef std::vector<MCDetector> TrackDetectorList;
typedef std::map<G4String,MCDetector*> VolumeMap;

class Magnetic_Field;

class myUserStepAction;
class myUserTrackAction;

class MCTracker
{
 private:
  //TrackDetectorList trackers;
  bool error;
  G4ErrorPropagatorManager *emgr;
  TrackDetectorConstruction *trackConstruction;
  G4ErrorFreeTrajState *traj;
  Magnetic_Field *pMF;
  //NoInterp_Magnetic_Field *pMF;
  G4ErrorTrackLengthTarget *lengthtarget;
  myUserStepAction *usa;
  G4UserSteppingAction *emptysa;
  myUserTrackAction *uta;

 public:
  // Overload constructor to allow setting of field file in constructor
  MCTracker(G4String GDMLfile, const char * FieldFile=NULL, int grid_type=1);
  ~MCTracker();

  VolumeMap vm;
  void initialize();
  void finalize();
  void setField(double scale);
  double getField();
  void getFieldVector(const G4ThreeVector R, G4ThreeVector *B);
  void setup(G4String,G4ThreeVector,G4ThreeVector);
  void flip(); //flips the momentum;
  G4ErrorSurfaceTrajState trackTo(MCDetector *,G4ErrorMode,int *error=NULL,double *tof=NULL);
  G4ErrorFreeTrajState trackDis(G4ErrorMode,int *err=NULL);
  void setTrackToList(std::vector<MCDetector *>);
  int trackToList(G4ErrorMode, int *error=NULL,double *tof=NULL,double *hitpos=NULL);
  //  void track(G4String, G4ThreeVector, G4ThreeVector);

 

};

#endif 
