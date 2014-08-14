#ifndef __CHEF_H__
#define __CHEF_H__

#include <vector>
#include <map>

#include "Plugin.h"
#include "RecipeReader.h"

#include "TMethodCall.h"
#include "TTree.h"



///
// This class is a helper class for all cooker type applications
//
class callinfo
{
  TMethodCall mc;
  Plugin *plug;
public:
  callinfo(Plugin * p,TClass *cl, std::string method);
  Long_t execute();
  //Long_t IsValid();
};

class Chef
{
 private:
  std::map<std::string,Plugin *> plugins;
  std::map<std::string, TObject *>repo;
  std::vector<callinfo*> cldefineHistograms,clstartup,clexecute,clfinalize;
  std::vector<callinfo*> compilelist(std::vector<class_method> &list);

  double weight;
  bool treeprint;
  int lastresult;
  int genskip,genseed,globseed;

 public:
  RecipeReader recipe;
  TTree *in,*out;
  TFile *infile,*outfile;

  Long_t printTrees(bool yn);

  Chef(std::string recipename,unsigned int seed=0,unsigned int gskip=0, unsigned int gseed=0);
  void loadPlugins();
  void processInit(int debug,std::map<std::string,std::vector<std::pair<std::string,std::string> > > pluginoptions);
  void prepareTreesExplora(TTree *in, std::string output);

  void prepareTrees(std::string input, std::string output);
  void startup();
  void defineHistograms();
  int processEvent(int i);
  void finalize();
  void addRepo(std::string name,TObject *obj);

  };


#endif
