#ifndef __COOKER_RM_
#define __COOKER_RM_

#include "G4RunManager.hh"


class cookerRM: public G4RunManager
{
 public:
 cookerRM():G4RunManager(){}
 bool startRun(); //this is the first half of the BeamOn loop
 void endRun(); // this is the second half of the BeamOn loop
 void doEvent(int eventNr); //do ONE event. Replacement for DoEventLoop

};

#endif
