#include <Det_Macro.h>

#include<iostream>
#include<cmath>
#include"TCint.h"

Det_Macro::Det_Macro(TTree *in, TTree *out,TFile *inf_, TFile * outf_, TObject *p):Plugin(in,out,inf_,outf_,p)
{
  macroname="";
  startupped=false;
  if (asprintf(&thisstring,"%p",this)==-1)
    {
      debug(0,"Macro: asprintf failed!?!?! Memory exhausted?\n");
      exit(-1);
    };
};

Det_Macro::~Det_Macro()
{
  free(thisstring);
};


Long_t Det_Macro::cmdline(char *cmd)
{
  //add cmdline hanling here

  return 0; // 0 = all ok
};


Long_t Det_Macro::filename(char *filename)
{
  macroname=filename;
  return 0;
}


Long_t Det_Macro::defineHistograms()
{
  if (macroname=="")
      {
      std::cerr<<"Macro: You have to set a macro filename!"<<std::endl;
      return -1;
    }
  
  if (gCint->LoadFile(macroname.c_str()))
    {
      std::cerr<<"Macro: Could not load macro: "<<macroname<<std::endl;
      return -2;
    }
  startupped=true;
  int erg;
  returnvalue=0;
  gCint->Execute("defineHistograms",thisstring,&erg);
  return returnvalue;
}

Long_t Det_Macro::startup()
{
  if (!startupped)
   
 {  if (macroname=="")
      {
      std::cerr<<"Macro: You have to set a macro filename!"<<std::endl;
      return -1;
    }
  
  if (gCint->LoadFile(macroname.c_str()))
    {
      std::cerr<<"Macro: Could not load macro: "<<macroname<<std::endl;
      return -2;
    }
  startupped=true;
    }
  int erg;
  returnvalue=0;
  gCint->Execute("startup",thisstring,&erg);
  
  return returnvalue;
}


Long_t Det_Macro::process()
{
  if (!startupped)
    {
      std::cerr<<"Macro: You have to call startup first!"<<std::endl;
      return -3;
    }
  int erg;
  gCint->Execute("process",thisstring,&erg);
  return erg;
}

Long_t Det_Macro::finalize()
{
  if (!startupped)
    {
      std::cerr<<"Macro: You have to call startup first!"<<std::endl;
      return -3;
    }
  int erg;
  returnvalue=0;
  gCint->Execute("finalize",thisstring,&erg);
  return returnvalue;
}


extern "C"{
Plugin *factory(TTree *in, TTree *out,TFile *inf_, TFile * outf_, TObject *p)
{
  return (Plugin *) new Det_Macro(in,out,inf_,outf_,p);
}
}


ClassImp(Det_Macro);

