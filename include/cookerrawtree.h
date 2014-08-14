/*
 * This is the definition of the cooker raw data structures
 * CRT = Cooker Root Tree
 */

#ifndef __COOKERRAWTREE_H__
#define __COOKERRAWTREE_H__


#include <TObject.h>
#include <TTimeStamp.h>
#include <vector>
#include <map>

/**
 * Base class for all components. We may use this for function prototypes.
 */

class CRTBase: public TObject
{
 public:
  CRTBase();
  virtual ~CRTBase();
  ClassDef(CRTBase,1);
};


/**
 * A class to store daq config files
 */

class CRTConfigFile:public CRTBase
{
 public:
  std::string filename;
  std::string content;
  CRTConfigFile();
  virtual ~CRTConfigFile();
  ClassDef(CRTConfigFile,1);
};



/**
 * A class to store information of the current run
 */

class CRTRunInfo:public CRTBase
{
	public:
		UInt_t runNumber;
		TTimeStamp startTime;
		TTimeStamp stopTime;
		UInt_t nrOfEvents;
		UInt_t activeLEVBS;
		// currently unused values
		Double_t targetFlow;
		Double_t magnetCurrent;
		Int_t beamSpecies; // -1=Electrons , 0=Cosmics , 1=Positrons
		CRTRunInfo();
		virtual ~CRTRunInfo();
		ClassDef(CRTRunInfo,2);
};

// Per event info 

class CRTEventInfo:public CRTBase
{
	public:
		UInt_t eventNumber;
		UInt_t eventType;
		TTimeStamp eventTime;
		UInt_t trigFired;
		UInt_t eventCounter;
		UInt_t resetCounter;
		UInt_t liveTimeCounter;
		UInt_t freeClock;
		UInt_t deadTimeCounter;
		double weight;
		CRTEventInfo();
		virtual ~CRTEventInfo();
		ClassDef(CRTEventInfo,2);
};

/**
 * A Class saving a binary blob, with a pointer and a size
 */

class CRTBinaryBlob:public CRTBase
{
 public:
  Int_t size; 
  UChar_t *data; //[size]
  CRTBinaryBlob();
  virtual ~CRTBinaryBlob();
  ClassDef(CRTBinaryBlob,1);
};

#endif

