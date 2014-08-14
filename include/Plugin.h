#ifndef __PLUGIN_H_
#define __PLUGIN_H_
#include "TObject.h"
#include "TTree.h"
#include "TFile.h"


class rng;



class TGCompositeFrame;
class TH1D;
class TH2D;
class TH3D;

class DetectorFlags:public TObject
{
 public:
  std::map<std::string, unsigned int> map;
  DetectorFlags();
  virtual ~DetectorFlags();
  ClassDef(DetectorFlags,1);
};


class FrameworkCallbacks
{
 public:
  virtual void addHisto(const char *path,TH1D *h)=0;
};

class Plugin:public TObject
{
 public:
  enum EventFlags {ok=0, maySkip=0x1000,skip=0x2000,stop=0x4000,redo=0x8000};
  rng * random;
  TTree *in, *out;
  TFile *infile, *outfile;
  std::map<std::string,TObject *> *lib;
  int debuglevel;
  Plugin(TTree *in_, TTree *out_, TFile *inf, TFile *outf,TObject *p_);
  void debug(int dl, const char * format, ...);
  void setDebug(int dl);
  void getBranchObject(const char * name,TObject **p);
  void getOutBranchObject(const char * name,TObject **p);
  void getBranchObjectFrom(TTree *from,const char * name,TObject **p);
  void setDetectorFlag(std::string name, unsigned int flag);
  void saveInfo (std::string name, double value);
   
  double getWeight(); // returns weight
  double setWeight(double); //sets new weight,returns old weight
  double addWeight(double ); //adds an additional weight by multiplying the current weight, returns old weight

  TH1D *dH1(const char * ,const char *, int, double, double);
  TH2D *dH2(const char * ,const char *, int, double, double,int, double, double);
  TH3D *dH3(const char * ,const char *, int, double, double, int, double, double, int, double, double);

  void H1(double, const char * , const char *, int, double, double);
  void H2(double, double, const char * ,const char *, int, double, double,int, double, double);
  void H3(double, double, double, const char * ,const char *, int, double, double, int, double, double, int, double, double);
  void cd(const char * dir);
 
 TGCompositeFrame * addTab(const char *name);

  TObject *getFileObject(const char* name);
  TObject *getMemoryObject(std::string name);
  void setMemoryObject(std::string name, TObject *p);
  void makeBranch(const char *name,TObject **p);
  Long_t copyBranch(const char* name);
  Long_t copyBranch_gracefully(const char* name);
  Long_t copyFileObject(const char* name);
  virtual Long_t cmdline(char * cmd)=0;

 private:
  double * weight;
  double dummyweight;
  const char *mkdirs(char *);
  void saveValue(const char *,const char *,const char *);
};


#endif
