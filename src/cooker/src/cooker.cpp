
#include "Plugin.h"

#include <iostream>
#include <sstream>
#include <boost/program_options.hpp>
#include <sys/time.h>
#include <algorithm>
#include "chef.h"

#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/format.hpp>

using boost::asio::ip::udp;


timeval costarttime;

double getdifftime()
{
  timeval tv;
  gettimeofday(&tv,0);
  return (tv.tv_sec-costarttime.tv_sec)*1000+(tv.tv_usec-costarttime.tv_usec)*1.0/1000;
}


namespace po = boost::program_options;

int main(int argc, char **argv)
{
  //setup options
  po::options_description desc("Cooker -- Raw to cooked with a recipe\nSyntax: cooker [options] <recipe> <source tree> <target tree>\n\tAllowed options:");
  desc.add_options()
    ("help,h","print help message")
    ("noprint,N","suppress ROOT tree printing at end")
    ("init,i",po::value<std::string> (),"override initialization xml file")
    ("start,s",po::value<unsigned int> ()->default_value(0),"Number of first event to process (starts with 0)")
    ("split,S",po::value<unsigned int>(),"Split in this many parts")
    ("part,p",po::value<unsigned int>(),"do this part")
    ("num,n",po::value<unsigned int> (),"Number of events to process")
    ("recipe,r",po::value<std::string>(),"recipe")
    ("input_tree,I",po::value<std::string>(),"input root tree")
    ("output_tree,O",po::value<std::string>(),"output root tree")
    ("verbose,v",po::value<int>()->implicit_value(1),"Verbose mode (optionally specify level)")
    ("every,e",po::value<int>()->default_value(500),"Print out status every x events")
    ("call,c",po::value<std::vector<std::string> >(),"Call a plugin's function. Needs argument with format: <plugin>:<function>:<arguments>") 
    ("monitor,m",po::value<std::string>(),"send monitor udp to this host (port 5555)")
    ("seed",po::value<unsigned int>()->default_value(0),"Set seed for detectors (0: read from /dev/random)")
    ("gseed",po::value<unsigned int>()->default_value(0),"Set seed for generators (0: read from /dev/random)")

    ("gskip",po::value<unsigned int>()->default_value(0),"Set skip for generators (default: 0)")
    ("remote","Switches on remote mode -- internal use only");

  po::positional_options_description pod;
  pod.add("recipe",1).add("input_tree",1).add("output_tree",1);
  po::variables_map vm;
  po::parsed_options parsopt=po::command_line_parser(argc, argv).options(desc).positional(pod).allow_unregistered().run();
  po::store(parsopt, vm); 
  po::notify(vm);
  if (vm.count("help") ||( vm.count("recipe")!=1 )|| (vm.count("input_tree")!=1))
    {
      std::cerr<<desc<<std::endl;
      return 1;
    }

  int every=vm["every"].as<int>();

  if (vm.count("split")!=vm.count("part"))
    {
      std::cerr<<"Need both split and part, or neither\n";
      exit(-100);
    }

  int debug= 0;

  if (vm.count("verbose"))
    {
      debug=vm["verbose"].as<int>();
      std::cerr<<"Setting debug level to:"<<debug<<std::endl;
    }

  Chef chef(argv[1],vm["seed"].as<unsigned int>(),vm["gskip"].as<unsigned int>(),vm["gseed"].as<unsigned int>());

  if (vm.count("noprint")) chef.printTrees(false);

  // init file  
  if (vm.count("init"))
    chef.recipe.InitXML=vm["init"].as<std::string>();
   
  std::cerr<<"Init file:  "<<chef.recipe.InitXML<<std::endl;

  bool remote=false;
  if (vm.count("remote"))
    {
      remote=true;
      char *tmpfilename=tempnam(NULL,"cooker.temp");
      std::cout<<"##REMOTE## filename "<<tmpfilename<<" \n";
      chef.prepareTrees(vm["input_tree"].as<std::string>(),tmpfilename);
    }
      else{
	if (vm.count("output_tree"))  
	  chef.prepareTrees(vm["input_tree"].as<std::string>(),vm["output_tree"].as<std::string>());
	else
	  chef.prepareTrees(vm["input_tree"].as<std::string>(),"");
      }

  bool sendudp=false;
  boost::asio::io_service io_service;
  boost::asio::ip::udp::socket socket(io_service);
  boost::asio::ip::udp::resolver resolver(io_service);
  boost::asio::ip::udp::endpoint dest_endpoint;
  if (vm.count("monitor"))
    {
    
      dest_endpoint= *resolver.resolve(udp::resolver::query(udp::v4(),vm["monitor"].as<std::string>(), "5555"));
      socket.open(dest_endpoint.protocol());
      sendudp=true;
    }

  chef.loadPlugins();



  // make map from unknown options
  std::map<std::string,std::vector<std::pair<std::string,std::string> > > pluginoptions;
  for (std::vector<po::basic_option<char> >::iterator iter=parsopt.options.begin();iter!=parsopt.options.end();iter++)

    if (iter->unregistered )
      if  (iter->value.size()==1)
	{
	  pluginoptions[iter->string_key].push_back(std::pair<std::string,std::string>("cmdline",iter->value[0]));
	}
      else
	std::cerr<<"Unknown token:"<< iter->string_key<<std::endl;

    
  if (vm.count("call"))
    {
      std::vector<std::string> ol=vm["call"].as<std::vector<std::string> >();
      for (std::vector<std::string>::iterator iter=ol.begin();iter!=ol.end();iter++)
	{
	  int part1, part2;
	  part1=iter->find(":");
	  part2=iter->rfind(":");
	  if (part2>part1)
	    {
	      std::cerr<<iter->substr(0,part1)<<" - "<<iter->substr(part1+1,part2-part1-1)<<" - "<<iter->substr(part2+1,iter->length())<<std::endl;
	      pluginoptions[iter->substr(0,part1)].push_back(std::pair<std::string,std::string>(iter->substr(part1+1,part2-part1-1),iter->substr(part2+1,iter->length())));
	    }
	  else
	    {std::cerr<<"Could not parse option "<<*iter<<std::endl;
	      exit(-20);
	    }
	}	   
    }

  chef.processInit(debug,pluginoptions);


  std::cerr<<"--Define Histograms--"<<std::endl;
  chef.defineHistograms();
  std::cerr<<"--Startup--"<<std::endl;
  chef.startup();

  std::cerr<<"--Looping--"<<std::endl;
  gettimeofday(&costarttime,0);

  int count=chef.in->GetEntries();
  int num=count;
  int start=vm["start"].as<unsigned int>();
  if (vm.count("num"))
    num=vm["num"].as<unsigned int>();
  if (vm.count("split"))
    {
      num=(int) (0.5+num*1.0/vm["split"].as<unsigned int>());
      start=num*(vm["part"].as<unsigned int>());

    }


  count=std::max(std::min(count-start,num),0);




  unsigned int ltdiff=0;
  for (int i=0;i<count;i++)
    { 
      int code=chef.processEvent(i+start);
      if (debug>=100 && code!=0)
	std::cerr<<"Loop gave:"<<code<<std::endl;

      if ( (i % every) ==0  || sendudp) {
	unsigned int tdiff=getdifftime();
	unsigned int cps=(unsigned int) (i*1.0/tdiff*1000.0);
	unsigned int eta= (unsigned int) (tdiff*1.0*(count-i)/i/1000);
	tdiff/=1000;
	if( (i %every)==0)
	  {
	    std::cerr<<"\r"<<i<<"/"<<count<<" @ "<<i+start<<" " <<i*100/count<<"%  "<<cps<<" Hz   ETA: ";
	    if (eta / 3600 >0) std::cerr<<eta/3600<<"h ";
	    if (eta / 60 >0) std::cerr<<eta % 3600 / 60<<"min ";
	    std::cerr<<eta % 60<<"s  Elapsed: ";
	    if (tdiff / 3600 >0) std::cerr<<tdiff/3600<<"h ";
	    if (tdiff / 60 >0) std::cerr<<tdiff % 3600 / 60<<"min ";
	    std::cerr<<tdiff % 60<<"s     "<<std::flush;
	  }
	
	if (sendudp && (tdiff>ltdiff))
	  {
	    ltdiff=tdiff;
	    std::string msg = boost::str(boost::format("%s: ETA:%is RT:%is\n") % vm["input_tree"].as<std::string>() % eta % tdiff);
	    socket.send_to(boost::asio::buffer(msg.c_str(), msg.size()), dest_endpoint);
	  }
	


	if (remote)
	  std::cout<<"\n##REMOTE## stat "<<i<<" "<<count<<" "<<cps<<" "<<eta<<std::endl;
      }

      if (code & Plugin::redo) i--;
    }
  
  
  std::cerr<<"\r"<<count<<"/"<<count<<" "<<100<<"%                       "<<std::flush;


  std::cerr<<std::endl<<"--Finalize--"<<std::endl;

  chef.finalize();
  


  std::cerr<<"--Done--"<<std::endl;

  return 0;
}
