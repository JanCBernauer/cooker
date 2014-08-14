
#include <iostream>
#include <boost/program_options.hpp>
#include<vector>
#include "TFile.h"
#include "TList.h"
#include "TKey.h"
#include "TClass.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TTree.h"
#include "TChain.h"

#include <fstream>
#include <set>

// #include "sclumi.h"
namespace po = boost::program_options;


class hisinfo
{
public:
  std::string path;
  TH1* his;
};


std::map< TString, hisinfo> histos;
std::set<TString> filled;

void chdir(TFile *f, std::string path)
{
  f->cd(); // go back to root
  std::stringstream ss(path);
  std::string item;
  while(std::getline(ss, item, '/')) 
    if (item!="")
      {
	if (!gDirectory->GetDirectory(item.c_str()))
	    gDirectory->mkdir(item.c_str());
	f->Cd(item.c_str());
      }
}


void recurse(std::string path,double lum)
{
  TList* list =gDirectory->GetListOfKeys() ;
  TIter next(list);
  TKey *key;
  TObject* obj ;
  std::string lastname = "BOWZINGLES";
  
  while ( key = (TKey*)next() ) {
    obj = key->ReadObj() ;
    if (obj->InheritsFrom("TDirectory"))
      { 
	gDirectory->cd(obj->GetName());
	recurse(path+"/"+obj->GetName(),lum);
	gDirectory->cd("..");
      }

    TString hname=path+"/"+obj->GetName();
    if (obj->InheritsFrom("TH1") && (filled.count(hname)==0))
      {
	filled.insert(hname);
	if ( histos.find(hname) !=histos.end())
	  {
	    histos[hname].his->Add((TH1*) obj,1);
	    histos[hname].his->SetNormFactor(histos[hname].his->GetNormFactor()+lum);
	    delete obj;
	  }
	else
	  {
	    ((TH1*) obj)->SetDirectory(0);
	    histos[hname].path=path;
	    histos[hname].his=(TH1*) obj;
	    histos[hname].his->SetNormFactor(lum);
	  }
      }

    else
      delete obj;
 
}
  
}

int main(int argc, char **argv)
{
  //setup options
  po::options_description desc("Combine -- Combine Histograms and export to gnuplot\nSyntax: combine [options] <input files>\n\tAllowed options:");
  desc.add_options()
    ("help,h","print help message")
    ("merge,m",po::value<std::string>(),"Merge trees with given name")
    ("root,R",po::value<std::string> (),"export histograms to root file")
    ("gnuplot,G",po::value<std::string> (),"export histograms to gnuplot file")
    ("only,o",po::value< std::vector<std::string> > (),"only export these histograms")
    ("normalize,N","Normalize to slow control luminosity (needs SCLumiInfo in *all* files)")
    ("single,S","For gnuplot files: single histogram per file, use -G option for directory")
    ("inputfile,i",po::value< std::vector<std::string> > (),"input file")
    ;
  
  po::positional_options_description p;
  p.add("inputfile", -1);
  
  po::variables_map vm;
  po::parsed_options parsopt=po::command_line_parser(argc, argv).options(desc).positional(p).run();
  po::store(parsopt, vm); 
  po::notify(vm);
  if (vm.count("help") || ( vm.count("root")==0 && vm.count("gnuplot")==0))
    {
      std::cout<<desc<<std::endl;
      return 1;
    }
  
  bool only=vm.count("only")>0;

  std::set<TString> onlyset;

  if (only)
    {
    std::vector<std::string> onlys=vm["only"].as<std::vector<std::string> >();
    for (unsigned int i=0;i<onlys.size();i++)
	   onlyset.insert(onlys[i]);
    }

  bool normalize=vm.count("normalize")>0;
  if (normalize) std::cout<<"Will normalize to slow control luminosity\n";

  std::string treename="";
  if (vm.count("merge")>0)
    treename=vm["merge"].as<std::string>();
      
  TChain chain(treename.c_str());

  std::vector< std::string > filelist=vm["inputfile"].as< std::vector<std::string> >();
  for (unsigned int i=0;i<filelist.size();i++)
    {
      std::cout<<"Reading "<<filelist[i]<<std::endl;
      TFile *f=new TFile(filelist[i].c_str(),"READ");
      if (vm.count("merge")>0)
	{
	  if (TTree *tree =(TTree *)f->Get(vm["merge"].as<std::string>().c_str()))
	    {
	      delete tree;
	      chain.Add(filelist[i].c_str());
	      //	      trees.Add(tree);
	    }
	  else
	    std::cout<<"File "<<filelist[i]<<" has no tree named "<<vm["merge"].as<std::string>()<<std::endl;
	}

      if (normalize)
	{
	  //	  SCLumiInfo *li=(SCLumiInfo*)f->Get("SCLumiInfo");
	  if (true)
	    std::cout<<"Skipping file "<<filelist[i]<<": No slowctrl info\n";
	  else
	    {
	      //	      std::cout<<"Luminosity:"<<li->dtcLuminosity<<std::endl;
	      filled.clear();
	      //	      recurse("",li->dtcLuminosity);
	    }
	}
      else
	{
	  filled.clear();
	  recurse("",0);
	}
      f->Close();
      delete f;
    }



  if (vm.count("root"))
    {
      std::cout<<"Writing histograms to root file: "<<vm["root"].as<std::string>()<<std::endl;
      TFile *f=new TFile(vm["root"].as<std::string>().c_str(),"RECREATE");

      for (std::map<TString,hisinfo>::iterator it=histos.begin();it!=histos.end();it++)
	{
	  chdir(f,it->second.path);
	  it->second.his->Write();
	}
      f->cd();
      f->cd("/");
      if (vm.count("merge")>0)
	{
	  std::cout<<"Merging trees... this might take a while\n";
	  chain.Merge(f,0,"fast");
	}
      else
	f->Close();
    }

  if (vm.count("gnuplot"))
    {
      std::cout<<"Writing histograms to gnuplot file: "<<vm["gnuplot"].as<std::string>()<<std::endl;
      double norm=1;
      ofstream *out;
      if (vm.count("single")==0)
	{
	out =new ofstream (vm["gnuplot"].as<std::string>().c_str());
	*out<<"#Directory:\n";
	}
      int i=0;

      for (std::map<TString,hisinfo>::iterator it=histos.begin();it!=histos.end();it++)
	if ( !only || onlyset.count(it->first)>0)
	  {
	  if (vm.count("single"))
	    {
	      char name[1000];
	      sprintf(name,"%s/%i",vm["gnuplot"].as<std::string>().c_str(),i);
	      out=new ofstream (name);
	    }
	  *out<<"#Index "<<i++<<": "<<it->first<<" "<<it->second.his->GetName()<<" "<<it->second.his->IsA()->GetName()<<std::endl;
	  if (vm.count("single"))
	    {
	      out->close();
	      delete out;
	    }
	  }
      
      i=0;
      for (std::map<TString,hisinfo>::iterator it=histos.begin();it!=histos.end();it++)
	{
	  if ( only && onlyset.count(it->first)==0) continue;
	  if (vm.count("single"))
	    {
	      char name[1000];
	      sprintf(name,"%s/%i",vm["gnuplot"].as<std::string>().c_str(),i++);
	      out=new ofstream (name,std::ios::app);
	    }
	  *out<<"# "<<it->second.his->GetName()<<std::endl;
	  if (normalize)
	    norm=it->second.his->GetNormFactor();

	  if (it->second.his->InheritsFrom("TH3"))
	    {
	      *out<<"# 3d histos not possible with gnuplot"<<std::endl;
	      
	      // 3 dim histo
	      /*      TH3 *h=(TH3*) it->second;
	      for (int z=0;z<=h->GetNbinsZ()+1;z++)
		for (int y=0;y<=h->GetNbinsY()+1;y++)
		  {
		    for (int x=0;x<=h->GetNbinsX()+1;x++)
		      out<<h->GetXaxis()->GetBinCenter(x)<<" "<<h->GetYaxis()->GetBinCenter(y)<<" "<<h->GetZaxis()->GetBinCenter(z)<<" "<<h->GetBinContent(x,y,z)<<std::endl;
		      out<<std::endl;
		}
	      */
	    }
	  else
	  if (it->second.his->InheritsFrom("TH2"))
	    {
	      // 2 dim histo
	      TH2 *h=(TH2*) it->second.his;
	      for (int y=0;y<=h->GetNbinsY()+1;y++)
		{
		  for (int x=0;x<=h->GetNbinsX()+1;x++)
		    *out<<h->GetXaxis()->GetBinCenter(x)<<" "<<h->GetYaxis()->GetBinCenter(y)<<" "<<h->GetBinContent(x,y)<<" "
			<<h->GetBinError(x,y)<<" "<<norm<<std::endl;
		    *out<<std::endl;
		}
	    }
	   else
	     {
	       TH1 *h=it->second.his;
	       for (int x=0;x<=h->GetNbinsX()+1;x++)
		 *out<<h->GetXaxis()->GetBinCenter(x)<<" "<<h->GetBinContent(x)<<" "<<h->GetBinError(x)<<" "<<norm<<std::endl;
	       *out<<std::endl;
	     }   
	  *out<<std::endl; // next record
	  if (vm.count("single"))
	    {
	      out->close();
	      delete out;
	    }
	}


      out->close();

    }



  return 0;
}
