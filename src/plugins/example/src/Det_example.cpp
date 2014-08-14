#include <Det_example.h>

#include <iostream>
#include <cmath>

// Constructor/destructor
Det_example::Det_example(TTree *in, TTree *out,TFile *inf_, TFile * outf_, TObject *p):Plugin(in,out,inf_,outf_,p)
{
};

Det_example::~Det_example()
{
};

// Standard recipe functions
//Long_t Det_example::startup()
//{
//   return 0;
//};

//Long_t Det_example::process()
//{
//   return 0;
//};

//Long_t Det_example::finalize()
//{
//   return 0;
//};

// Cooker command line option function
Long_t Det_example::cmdline(char *cmd)
{
   // Add command line handling flags here

   return 0;
};


extern "C"
{
   Plugin *factory(TTree *in, TTree *out,TFile *inf_, TFile * outf_, TObject *p)
   {
      return (Plugin *) new Det_example(in,out,inf_,outf_,p);
   }
}

ClassImp(Det_example);
