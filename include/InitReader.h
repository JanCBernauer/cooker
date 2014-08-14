#ifndef _INITREADER_H_
#define _INITREADER_H_

#include <string>
#include <map>
#include <vector>
#include <xqilla/xqilla-simple.hpp>


/**
 *  This library parses xml-style init files
 **/



class InitReader
{
 private:
  XQilla xqilla;
  XQQuery *querynr;
  XQQuery *querydate;
  DynamicContext *context;
  XercesConfiguration xc;
 public:
  InitReader(std::string filename,std::string date,int runnr); // Read XML-file named filename 
  ~InitReader();
  std::map<std::string,std::vector<std::string> > getConfig(std::string name); // Return config strings for 
  
};



#endif 
