#include <Det_Copy.h>

#include<iostream>
#include<cmath>


Det_Copy::Det_Copy(TTree *in, TTree *out,TFile *inf_, TFile * outf_, TObject *p):Plugin(in,out,inf_,outf_,p)
{
};

Det_Copy::~Det_Copy()
{
};


Long_t Det_Copy::copy()
{
  for (std::map<int,std::string>::iterator iter=branchnames.begin();iter!=branchnames.end();iter++)
    {
      std::cout<<"Calling for NR:"<<iter->first<<" Name: >"<<iter->second<<"<"<<std::endl;
      copyBranch(iter->second.c_str());
    }
  for (std::map<int,std::string>::iterator iter=objnames.begin();iter!=objnames.end();iter++)
    copyFileObject(iter->second.c_str());
  return 0;
}

Long_t Det_Copy::branch(int id,char * name)
{
  std::cout<<"Name to add: >"<<name<<"<"<<std::endl;
  debug(99,"COPY: Add branch with name %s\n",name);
  branchnames[id]=name;
  return 0;
}

Long_t Det_Copy::obj(int id,char * name)
{
  debug(99,"COPY: Add obj with name %s\n",name);
  objnames[id]=name;
  return 0;
}

Long_t Det_Copy::cmdline(char *cmd)
{
  //add cmdline hanling here

  return 0; // 0 = all ok
};


extern "C"{
Plugin *factory(TTree *in, TTree *out,TFile *inf_, TFile * outf_, TObject *p)
{
  return (Plugin *) new Det_Copy(in,out,inf_,outf_,p);
}
}


ClassImp(Det_Copy);

