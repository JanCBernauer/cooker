#ifndef __DET_EXAMPLE__
#define __DET_EXAMPLE__

#include "TObject.h"
#include "Plugin.h"
#include "TTree.h"
#include <iostream>

class Det_example:public Plugin
{
   private:


   public:
      Det_example(TTree *in, TTree *out,TFile *inf_, TFile * outf_, TObject *p);
      virtual ~Det_example();
      // add funtions with return value Long_t here:

//      Long_t startup();
//      Long_t process();
//      Long_t finalize();

      virtual Long_t cmdline(char * cmd);

      ClassDef(Det_example,1);
};

#endif
