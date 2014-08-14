#include<map>
#include"TObject.h"
//wrapper for a map from string to TObject;
class genericmap: public TObject
{
 public:
  std::map< std::string, TObject *> map;
  virtual const char *  GetName() const {return "genericmap";} ;
};


