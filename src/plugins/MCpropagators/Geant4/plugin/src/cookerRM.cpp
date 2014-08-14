#include "cookerRM.h"


bool cookerRM::startRun()
{
  if (!ConfirmBeamOnCondition())
    {
      return false; 
    }
  ConstructScoringWorlds();
  RunInitialization();
  return true;
}

void cookerRM::endRun()
{
  RunTermination();
}

void cookerRM::doEvent(int eventNr)
{
  currentEvent=GenerateEvent(eventNr);
  eventManager->ProcessOneEvent(currentEvent);
  AnalyzeEvent(currentEvent);
  UpdateScoring();
  StackPreviousEvent(currentEvent);
  currentEvent=0;
}
