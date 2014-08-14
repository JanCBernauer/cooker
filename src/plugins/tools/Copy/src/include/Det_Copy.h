#ifndef __DET_COPY__
#define __DET_COPY__

#include "TObject.h"
#include "Plugin.h"
#include "TTree.h"
#include <iostream>
#include <map>

class Det_Copy:public Plugin
{
 private:
  std::map<int,std::string> branchnames;
  std::map<int,std::string> objnames;

 public:
  Det_Copy(TTree *in, TTree *out,TFile *inf_, TFile * outf_, TObject *p);
  virtual ~Det_Copy();
  // add funtions with return value Long_t here:
  Long_t branch(int id,char *name);
  Long_t obj(int id,char *name);
  Long_t copy();

  virtual Long_t cmdline(char * cmd);

  ClassDef(Det_Copy,1);
    };

#endif
