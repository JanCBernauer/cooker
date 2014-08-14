
#include "Plugin.h"

#include <iostream>
#include <sstream>
#include <boost/program_options.hpp>
#include <sys/time.h>
#include <algorithm>
#include "chef.h"

#include "TApplication.h" 
#include "controlwindow.h"

timeval starttime;



namespace po = boost::program_options;


int main(int argc, char **argv)
{


  po::options_description desc("visco, the Visual Cooker -- Raw to cooked with a recipe\nSyntax: visco [options] <recipe> <source tree> <target tree>\n\tAllowed options:");
  desc.add_options()
    ("help,h","print help message")
    ("init,i",po::value<std::string> (),"override initialization xml file")
    ("recipe,r",po::value<std::string>(),"recipe")
    ("input_tree,I",po::value<std::string>(),"input root tree")
    ("output_tree,O",po::value<std::string>(),"output root tree")
    ("verbose,v",po::value<int>()->implicit_value(1),"Verbose mode (optionally specify level)")
    ("call,c",po::value<std::vector<std::string> >(),"Call a plugin's function. Needs argument with format: <plugin>:<function>:<arguments>") 
    ("eventlist,e",po::value<std::string> (),"process only events from file with event list")
    ;
  po::positional_options_description pod;
  pod.add("recipe",1).add("input_tree",1).add("output_tree",1);
  po::variables_map vm;
  po::parsed_options parsopt=po::command_line_parser(argc, argv).options(desc).positional(pod).allow_unregistered().run();
  po::store(parsopt, vm); 
  po::notify(vm);
  if (vm.count("help") ||( vm.count("recipe")!=1 )|| (vm.count("input_tree")!=1))
    {
      std::cout<<desc<<std::endl;
      return 1;
    }


  argc=0;
  printf("A\n");
  TApplication theApp("VisualCooker",&argc,argv);
  printf("B\n");
  ControlWindow *mainWindow=new ControlWindow(gClient->GetRoot(),1000,480);
  printf("C\n");
  if (vm.count("verbose"))
    {
      mainWindow->debug=vm["verbose"].as<int>();
      std::cout<<"Setting debug level to:"<<mainWindow->debug<<std::endl;
    }

  

  mainWindow->chef= new Chef(vm["recipe"].as<std::string>());
  // init file  
  if (vm.count("init"))
    mainWindow->chef->recipe.InitXML=vm["init"].as<std::string>();
   
  std::cout<<"Init file:  "<<mainWindow->chef->recipe.InitXML<<std::endl;


  if (vm.count("output_tree"))  
    mainWindow->chef->prepareTrees(vm["input_tree"].as<std::string>(),vm["output_tree"].as<std::string>());
  else
    mainWindow->chef->prepareTrees(vm["input_tree"].as<std::string>(),"");


  mainWindow->chef->loadPlugins();

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
	      std::cout<<iter->substr(0,part1)<<" - "<<iter->substr(part1+1,part2-part1-1)<<" - "<<iter->substr(part2+1,iter->length())<<std::endl;
	      pluginoptions[iter->substr(0,part1)].push_back(std::pair<std::string,std::string>(iter->substr(part1+1,part2-part1-1),iter->substr(part2+1,iter->length())));
	    }
	  else
	    {std::cerr<<"Could not parse option "<<*iter<<std::endl;
	      exit(-20);
	    }
	}
    }

  // read file with list of eventnumbers if specified by user:
  if (vm.count("eventlist"))
    {
      std::string elfilename = vm["eventlist"].as<std::string>();
      std::cout << " ===== eventlist file " << elfilename << " specified! =====\n";
      FILE *elfile = fopen(elfilename.c_str(), "r");
      if (elfile)
	{
	  int eventnumber, dummy;
	  while (!feof(elfile))
	    {
	      dummy=fscanf(elfile, "%d\n", &eventnumber);
	      mainWindow->eventlist.push_back(eventnumber);
	    };
	  fclose(elfile);
	  printf(" >>> %d eventnumbers read from eventlist file!\n", int(mainWindow->eventlist.size()));
	}
      else
	{
	  perror("Cannot open eventlist file");
	};
    }
  // if something failed or no list was specified, disable buttons:
  if (mainWindow->eventlist.size()==0)
    {
      mainWindow->elfwdBtn->SetEnabled(false);
      mainWindow->elbwdBtn->SetEnabled(false);
    }

  mainWindow->chef->processInit(mainWindow->debug,pluginoptions);



  std::cout<<"--Startup--"<<std::endl;
  mainWindow->startup(vm["recipe"].as<std::string>());

  theApp.Run();

return 0;
}


