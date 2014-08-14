#ifndef __DET_MACRO__
#define __DET_MACRO__

#include "TObject.h"
#include "Plugin.h"
#include "TTree.h"
#include <iostream>

class Det_Macro:public Plugin
{
 private:
  std::string macroname;
  bool startupped;
  char *thisstring;
 public:
  Long_t returnvalue;
  Det_Macro(TTree *in, TTree *out,TFile *inf_, TFile * outf_, TObject *p);
  virtual ~Det_Macro();
  // add funtions with return value Long_t here:
  Long_t defineHistograms();
  Long_t startup();
  Long_t process();
  Long_t finalize();
  
  Long_t filename(char *filename);

  virtual Long_t cmdline(char * cmd);

  ClassDef(Det_Macro,1);
    };

#endif
