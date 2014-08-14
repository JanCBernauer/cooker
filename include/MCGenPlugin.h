#ifndef __MCGENPLUGIN_H_
#define __MCGENPLUGIN_H_
#include "TObject.h"
#include "TTree.h"
#include "TFile.h"
#include "Plugin.h"

class GeneratorBase;
class GeneratorEvent;
class TH1D;
class TH2D;

class TVector3;
namespace slowctrl
{
  class manager;
  class datum;
}

class SCLumiInfo;

class MCGenPlugin:public Plugin
{
 private:
  unsigned int numDataEvents;
  unsigned int numSimulEvents;
  int eventcounter;
  bool useQuasi;
  bool useBeamGeometrySlowCtrl;
  
  // Slow-Ctrl stuff
  slowctrl::manager* scman;
  slowctrl::datum* eDipole;

  // Lumi
 SCLumiInfo  *li;
  double oldluminosity;

  // Histograms
  TH1D * vertexHist;
  TH2D * beamProfileHist;
  TH2D * beamSlopeHist;


  bool beamChargeOverride;
  bool beamEnergyOverride;
  TVector3 *beamOrigin;
  TVector3 *beamDirection;  

 protected:
  GeneratorEvent *ge;
  GeneratorBase *generator;
  unsigned int getSkip();
  unsigned long int getSeed();
  bool newEvent;
  double luminosity;

 public:
  MCGenPlugin(TTree *in_,TTree *out_,TFile *inf_, TFile * outf_,TObject *p_);
  
  virtual Long_t cmdline(char * cmd)=0;
  virtual Long_t startup();   
  virtual Long_t process();
  virtual Long_t finalize();

  Long_t useQuasiRandom(bool);
  Long_t genSameEvent(bool);
  Long_t setEventCount(unsigned int n);
  Long_t setBeamE(double energy);
  Long_t setBeamSpecies(int charge);
  Long_t setBeamOffset(double mmX, double mmY, double mmZ);
  Long_t setBeamSlope(double xSlope, double ySlope);
  Long_t setVertexDist(char * dist);

  virtual Long_t genStartup()=0;
  virtual Long_t genProcess()=0;
  virtual Long_t genFinalize()=0;
  ClassDef(MCGenPlugin,1);
};


#endif
