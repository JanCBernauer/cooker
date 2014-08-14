#ifndef _RECIPEREADER_H_
#define _RECIPEREADER_H_

#include <string>
#include <vector>
#include <map>


/**
 *  This library parses xml-style recipe files
 **/



class class_method
{
 public:
  std::string classname;
  std::string method;
  class_method(std::string c, std::string m);
};

class RecipeReader
{
 public:
  std::string InitXML;
  std::string srctree;
  std::string dsttree;
  std::map<std::string,std::string>  plugins;
  std::vector<class_method> defineHistograms;
  std::vector<class_method> commands;
  std::vector<class_method> startup;
  std::vector<class_method> finalize;

  RecipeReader(std::string filename); // Read XML-file named filename
  ~RecipeReader();

  
};



#endif 
