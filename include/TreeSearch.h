#ifndef __TREESEARCH__
#define __TREESEARCH__

#include <string>
#include <fstream>
#include <vector>
#include <iostream>
#include <cstdlib>

template <class T, int levels>
class TreeSearch
{
 public:
  class entry
  {
  public:
    T pattern;
    unsigned int skipto;
  }__attribute__ ((aligned(16))) ;

 private:
  entry *entries; //pointer to first entry.
  T ptm[levels];

  void preparePtm(T& pat)
  {
    int shift=1;
    ptm[0]=pat;
    for (int i=1;i<levels;i++)
      {
	pat|=pat>>shift;
	shift*=2;
	ptm[i]=pat;
      }
    pat=ptm[0];
  }

  bool recurseFindMatch(int lv, entry *e)
  {
    while(true) 
      {
	if ((ptm[lv]& e->pattern)==e->pattern)
	  {
	    if (lv==0) return true;
	    if (recurseFindMatch(lv-1,e+1)) return true;
	  }
	if (e->skipto==0) return false;
	e=&entries[e->skipto];
      }
  }

  void recurseEnumerateMatch(int lv, entry *e)
  {
    while(true) 
      {
	if ((ptm[lv]& e->pattern)==e->pattern)
	  {
	    if (lv==0) 
	      // found a match, save it to library.
	      matches.push_back(&(e->pattern));
	    else
	      recurseEnumerateMatch(lv-1,e+1);
	  }
	if (e->skipto==0) return;
	e=&entries[e->skipto];
      }
  }


 public:
    std::vector<T*> matches;

  TreeSearch(std::string filename)
  {
    std::ifstream is;
    is.open (filename.c_str(), std::ios::binary );
    
	  if (!is.good())
	  {
		  std::cerr << "Did not load file properly.  Check the name.  \n";
		  std::cerr << "You called " << filename << std::endl;
		  std::cerr << "Don't be an ass-dan!\n";
		  std::exit(-1);
	  }
	  
    // get length of file:
    is.seekg (0, std::ios::end);
    int length = is.tellg()/sizeof(entry);
    is.seekg (0, std::ios::beg);

    // allocate memory:
    entries = new entry[length];

    // read data as a block:
    is.read ((char *)entries,length*sizeof(entry));
    is.close();
  }
 
  ~TreeSearch()
    {
      delete[] entries;
    }

  //recursive approach
  bool hasMatch(T& pat)
  {
    preparePtm (pat);
    return recurseFindMatch(levels-1,entries);
  }

void enumerateMatches(T& pat)
  {
    matches.clear();
    preparePtm (pat);
    recurseEnumerateMatch(levels-1,entries);

  }

 };

#endif

