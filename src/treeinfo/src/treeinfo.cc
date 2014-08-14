#include <iostream>
#include <cstdlib>
#include "TFile.h"
#include "TKey.h"
#include "TTree.h"
#include "cookerrawtree.h"

TTree t;
CRTBase dummy;

using namespace std;


int main( int argc, char **argv)
{
  if (argc!=2)
    {
      cerr<<"Usage: treeinfo <rootfile.root>\n";
      exit(-1);
    }

  TFile *thefile=new TFile(argv[1],"R");

  if (!thefile)
    {
      cerr <<"Could not open "<<argv[1]<<"\n";
      exit(-2);
    }

  cout<<"File "<<argv[1]<<" contains the following trees\n";

  TIter next(thefile->GetListOfKeys()); 

  TKey *key;
  while (key = (TKey*) next())
    {

      TObject * t=key->ReadObj();
      if (t->InheritsFrom("TTree"))
	{
	  TTree *tree=(TTree*)t;
	  cout<<key->GetName()<<" "<<key->GetCycle()<<"\n";
	  cout<<tree->GetName()<<" "<<tree->GetEntries()<<"\n\n";
	}
    }

  thefile->Close();

}
