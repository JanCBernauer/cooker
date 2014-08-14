
#include "Plugin.h"
#include "TBranchElement.h"
#include <iostream>
#include <cstdio>
#include <cstdarg>
#include <map>
#include "genericmap.h"

#include "TGTab.h"
#include "TError.h"

#include "TH1.h"
#include "TH2.h"
#include "TH3.h"

#ifdef WITHDB
#include <libpq-fe.h>
#include <arpa/inet.h>
#endif


#include "gsl/gsl_rng.h"
#include <cmath>
struct rng:public gsl_rng{};


class CH1D:public TH1D
{
private:
  double *weight;
public:
  CH1D(const char *a,const char* b, int c, double d, double e,double *w):TH1D(a,b,c,d,e),weight(w){Sumw2();};
 Int_t	Fill(Double_t x)
  {
    return TH1D::Fill(x,*weight);
  }
  Int_t	Fill(Double_t x,Double_t w)
  {
    return TH1D::Fill(x,w*(*weight));
  }

};


class CH2D:public TH2D
{
private:
  double *weight;
public:
  CH2D(const char *a,const char* b, int c, double d, double e, int f, double g, double h,double *w):TH2D(a,b,c,d,e,f,g,h),weight(w){Sumw2();};
  Int_t	Fill(Double_t x,Double_t y)
  {
    return TH2D::Fill(x,y,*weight);
  }
  Int_t	Fill(Double_t x,Double_t y,Double_t w)
  {
    return TH2D::Fill(x,y,w*(*weight));
  }

};


class CH3D:public TH3D
{
private:
  double *weight;
public:
  CH3D(const char *a,const char* b, int c, double d, double e, int f, double g, double h,int i, double j, double k,double *w):TH3D(a,b,c,d,e,f,g,h,i,j,k),weight(w){Sumw2();};
  Int_t	Fill(Double_t x,Double_t y,Double_t z)
  {
    return TH3D::Fill(x,y,z,*weight);
  }
  Int_t	Fill(Double_t x,Double_t y,Double_t z,Double_t w)
  {
    return TH3D::Fill(x,y,z,w*(*weight));
  }

};




Plugin::Plugin(TTree *in_, TTree *out_, TFile *inf_, TFile *outf_,TObject *p_)
{
  std::cout<<"Called constructor"<<std::endl;
  std::cout<<"....with Trees:"<<in_<<" "<<out_<<std::endl;
  std::cout<<"....with Files:"<<inf_<<" "<<outf_<<std::endl;
  std::cout<<"....with Object:"<<p_<<std::endl;
  
  in=in_;
  out=out_;
  infile=inf_;
  outfile=outf_;
  lib=(std::map<std::string,TObject *> *) p_;

  weight=(double *)getMemoryObject("Weight");
  debuglevel=0;

  random=(rng*) getMemoryObject("Random");
}




const char * Plugin::mkdirs(char * name)
{
  while (*name !=0)
    {
      const char * oldname=name;
      while( *name != '/' && (*name)!=0 ) name++;
      if (*name==0)
	return oldname;
      else
	{
	  *name=0;
	  name++;
	if (!gDirectory->GetDirectory(oldname))
	  gDirectory->mkdir(oldname);
	gDirectory->cd(oldname);
	}
    }
}

void Plugin::cd( const char *dir)
{
  TObjArray *dirs=TString(dir).Tokenize("/");
  for (int i=0;i<dirs->GetEntriesFast();i++)
    {
      if (!gDirectory->GetDirectory(((TObjString *) dirs->At(i))->GetString().Data()))
	gDirectory->mkdir(((TObjString *) dirs->At(i))->GetString().Data());
      gDirectory->cd(((TObjString *) dirs->At(i))->GetString().Data());
    }
  delete dirs;
}

TH1D *Plugin::dH1(const char * name, const char * title, int count, double from, double to)
{
  TH1D *hist=0;
  gDirectory->GetObject(name,hist);
  if (!hist)
    {
      const char * path=gDirectory->GetPath();
      TDirectory * curr=gDirectory->CurrentDirectory();
      char buf[1000];
      strlcpy(buf,name,1000);
      hist=new CH1D(mkdirs(buf),title, count, from,to,(double *)getMemoryObject("Weight"));
      gErrorIgnoreLevel = 2001;
      if (getMemoryObject("Callbacks"))  ((FrameworkCallbacks *)getMemoryObject("Callbacks"))->addHisto((std::string(path)+"/"+std::string(name)).c_str(),(TH1D *)hist);
      gErrorIgnoreLevel = 0;
      curr->cd();

    }
  return hist;
}

TH2D *Plugin::dH2(const char * name, const char * title, int c1, double f1, double t1, int c2, double f2, double t2)
{
 TH2D *hist=0;
  gDirectory->GetObject(name,hist);
  if (!hist)
    {
      const char * path=gDirectory->GetPath();
      TDirectory * curr=gDirectory->CurrentDirectory();
      char buf[1000];
      strlcpy(buf,name,1000);
      hist=new CH2D(mkdirs(buf),title, c1,f1,t1,c2,f2,t2,(double *) getMemoryObject("Weight"));
      gErrorIgnoreLevel = 2001;
      if (getMemoryObject("Callbacks"))  ((FrameworkCallbacks *)getMemoryObject("Callbacks"))->addHisto((std::string(path)+"/"+std::string(name)).c_str(),(TH1D *)hist);
      gErrorIgnoreLevel = 0;
      curr->cd();
    }
  return hist;
}

TH3D *Plugin::dH3(const char * name, const char * title, int c1, double f1, double t1, int c2, double f2, double t2, int c3, double f3, double t3)
{
 TH3D *hist=0;
  gDirectory->GetObject(name,hist);
  if (!hist)
    {
      const char * path=gDirectory->GetPath();
      TDirectory * curr=gDirectory->CurrentDirectory();
      char buf[1000];
      strlcpy(buf,name,1000);
      hist=new CH3D(mkdirs(buf),title, c1,f1,t1,c2,f2,t2,c3,t3,f3,(double *)getMemoryObject("Weight"));
      gErrorIgnoreLevel = 2001;
      if (getMemoryObject("Callbacks"))  ((FrameworkCallbacks *)getMemoryObject("Callbacks"))->addHisto((std::string(path)+"/"+std::string(name)).c_str(),(TH1D *) hist);
      gErrorIgnoreLevel = 0;
      curr->cd();
    }
  return hist;

}

void Plugin::H1(double val,const char * name, const char * title, int count, double from, double to)
{
 TH1D *hist=0;
 gDirectory->GetObject(name,hist); // this could be skipped, but it's faster this way
 if (!hist)
   hist=dH1(name,title,count,from,to);
 hist->Fill(val);
}

void Plugin::H2(double v1,double v2,const char * name, const char * title, int c1, double f1, double t1, int c2, double f2, double t2)
{
 TH2D *hist=0;
 gDirectory->GetObject(name,hist); // this could be skipped, but it's faster this way
 if (!hist)
   hist=dH2(name,title,c1,f1,t1,c2,f2,t2);
 hist->Fill(v1,v2);
}


void Plugin::H3(double v1, double v2, double v3,const char * name, const char * title, int c1, double f1, double t1, int c2, double f2, double t2, int c3, double f3, double t3)
{
 TH3D *hist=0;
 gDirectory->GetObject(name,hist); // this could be skipped, but it's faster this way
 if (!hist)
   hist=dH3(name,title,c1,f1,t1,c2,f2,t2,c3,f3,t3);
 hist->Fill(v1,v2,v3);
}




void Plugin::debug(int dl,const char * format,  ...)
{
  if (dl<=debuglevel)
    {
      va_list ap;
      va_start(ap,format);
      vfprintf(stderr, format,ap);
      va_end(ap);
    }
}

void Plugin::setDebug(int dl)
{
  debuglevel=dl;
}

TGCompositeFrame * Plugin::addTab(const char *name)
{
  TGTab *tabs=(TGTab *) getMemoryObject("Tab Widget");
  if (!tabs)
    return NULL;
  return tabs->AddTab(name);

}

void Plugin::getBranchObject(const char *name, TObject **p)
{
  return getBranchObjectFrom(in,name,p);
}

void Plugin::getOutBranchObject(const char *name, TObject **p)
{
  return getBranchObjectFrom(out,name,p);
}



void Plugin::getBranchObjectFrom(TTree *from,const char *name, TObject **p)
{
 from->SetBranchStatus(name,1);
 TBranchElement *br=(TBranchElement *)from->GetBranch(name);
 if (!br)
   {
     *p=NULL; // set pointer NULL explicitely to signal branch was not found!
     debug(1,"Branch %s not found\n",name);
     return;
   }
 *p=(TObject *)br->GetObject();
 if (*p==0)
   {
     from->SetBranchAddress(name,p); 
   }

 debug(100,"Got Branch: %s, Status: %i at %p\n",name,from->GetBranchStatus(name),*p);
}


TObject * Plugin::getFileObject(const char *name)
{

  if (!infile)
    return ((genericmap*) getMemoryObject("File Object Map"))->map[name];
  return infile->Get(name);
}


void Plugin::saveValue(const char *tagname, const char *tablename, const char *valstring)
{
#ifdef WITHDB
  PGconn *conn = NULL;

  TObject *dummy=getMemoryObject("Runnumber");
  int runnumber=htonl(*(int*) &dummy); //ugly but works 
  // Make a connection to the database
  conn = PQconnectdb("user=runinfo password=runinfo dbname=runinfo host=CookerWorkerNodeLNS00");
  
  // Check to see that the backend connection was successfully made
  if (PQstatus(conn) != CONNECTION_OK)
    {
      debug(0,"Connection to database failed\n");
      PQfinish(conn);
      return ;
    }
  const char *params[]={tagname,(char *) &runnumber,valstring};

  int lengths[3]={strlen(tagname),sizeof(runnumber),strlen(valstring)};
  int binary[3]={0,1,0};
  debug(100,"Connection to database - OK\n");
  PGresult *res=PQexecParams(conn, "select tagid from tagnames where tagname=$1::varchar;",1,NULL,params,lengths,binary,0);
  if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
      debug(0, "select tagid command failed: %s\n", PQerrorMessage(conn));
      PQclear(res);
      PQfinish(conn);
      return;
    }
  debug(100,"Found %i\n",PQntuples(res));
  if (PQntuples(res)==0)
    {
      PQclear(res);
      debug(100,"Trying to insert\n");
      res=PQexecParams(conn, "insert into tagnames  (tagname) values ($1::varchar);",1,NULL,params,lengths,binary,0);
      if (PQresultStatus(res) != PGRES_COMMAND_OK)
	{
	  debug(0, "insert into tagid command failed: %s\n", PQerrorMessage(conn));
	  PQclear(res);
	  PQfinish(conn);
	  return;
	}
    }
  PQclear(res);
  debug(100,"Trying to insert\n");
  char command[1000];
  sprintf(command,"insert into %s (timestamp,runid,tagid,value) select now(),$2::int4,tagid,$3::double precision from tagnames where tagname=$1::varchar;",tablename);
  res=PQexecParams(conn, command,3,NULL,params,lengths,binary,0);
  if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
      debug(100, "insert into rundata command failed: %s\n Trying update", PQerrorMessage(conn));
      PQclear(res);
      sprintf(command,"update %s set timestamp=now(), value=$3::double precision where runid=$2::int4 and tagid=(select tagid from tagnames where tagname=$1::varchar);",tablename);
      res=PQexecParams(conn, command,3,NULL,params,lengths,binary,0);
      if (PQresultStatus(res) != PGRES_COMMAND_OK)
	{
	  debug(100, "update failed too: %s\n", PQerrorMessage(conn));
	  PQclear(res);
	  PQfinish(conn);
	  return;
	}
    }
  PQclear(res);
  
  PQfinish(conn);
#else
  debug(10000,"DB not compiled in\n");
#endif
}

void Plugin::setDetectorFlag(std::string name, unsigned int flag)
{
  char valstring [1000];
  sprintf(valstring,"%i",flag);
  saveValue(name.c_str(),"detectorFlag",valstring);
} 



void Plugin::makeBranch(const char *name, TObject **p)
{
  out->Branch(name,p,16000,0); 
}

Long_t Plugin::copyBranch(const char *name)
{
  debug(10,"Plugin: Copying branch %s\n",name);
  TObject **p = new (TObject *);
  getBranchObject(name,p);
  makeBranch(name,p);
  return 0;
}
Long_t Plugin::copyBranch_gracefully(const char *name)
{
  debug(10,"Plugin: Copying branch %s 'gracefully'\n",name);
  TObject **p = new (TObject *);
  getBranchObject(name,p);
  if (*p)
    makeBranch(name,p);
  else
    debug(0, "Plugin: branch %s wasn't found - skipping it!\n", name);
  return 0;
}



Long_t Plugin::copyFileObject(const char *name)
{
  if (!outfile)
    {
      debug(0,"Plugin: trying to copy %s with no target file\n",name);
    return -1;
    }
  debug(10,"Plugin: Copying object %s\n",name);
  // do some stunts to repair root braindeadness
  //split path part
  std::string n,rn;
  if (name[0]!='/')
    n="/"+std::string(name);
  else
    n=std::string(name);
  std::string path=n.substr(0, n.find_last_of('/'));
  std::string objname=n.substr(n.find_last_of('/')+1);
  if (path=="") 
    rn=objname;  //root shits itself when requesting an object in / as /name
  else
    {
      path=path.substr(1);
      rn=path+"/"+objname;
    }
  debug(99,"Plugin: Split path is %s and name is %s\n",path.c_str(),objname.c_str());

  outfile->cd();
  if (!outfile->GetDirectory(path.c_str()) )
    outfile->mkdir(path.c_str());

  outfile->cd(path.c_str());
  getFileObject(rn.c_str())->Write(objname.c_str());
  outfile->cd();
  return 0;
}


TObject* Plugin::getMemoryObject(std::string name)
{
  if (!lib)
    {
      debug(0,"No memory object map provided\n");
      return NULL;
    }
  std::map<std::string,TObject *>::iterator res=lib->find(name);
  if (res == lib->end())
    return NULL;
  return res->second;
}

void Plugin::setMemoryObject(std::string name, TObject *p)
{
  if (!lib)
    {
      debug(0,"No memory object map provided\n");
      return;
    }
  (*lib)[name]=p;
}



double Plugin::getWeight()
{
  return *weight;
}

double Plugin::setWeight(double nw)
{
  double oldweight=*weight;
  *weight=nw;
  return oldweight;
}

double Plugin::addWeight(double nw)
{
  double oldweight=*weight;
  *weight=nw*oldweight;
  return oldweight;
}

void Plugin::saveInfo(std::string name, double value)
{
  char valstring [1000];
  sprintf(valstring,"%0.20g",value);
  saveValue(name.c_str(),"rundata",valstring);
}



DetectorFlags::DetectorFlags() {};
DetectorFlags::~DetectorFlags() {};
ClassImp(DetectorFlags);



ClassImp(Plugin);
