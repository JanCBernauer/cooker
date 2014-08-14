#include <TreeSearch.h>
#include <iostream>
#include <bitset>
using namespace std;

// this program reads a list of these
typedef  bitset<180> patType;



TreeSearch<patType,9> ts("pattern.dat");

int main (int argc , char **argv)
{
  patType pat=1;
  while (pat!=0)
    {
      cin>>hex>> pat;
      cout<<pat<<endl;
      cout<<ts.hasMatch(pat)<<endl;
      ts.enumerateMatches(pat);
      cout<<ts.matches.size()<<std::endl;
      for (unsigned int i=0; i<ts.matches.size();i++)
      	cout<< *ts.matches[i]<<std::endl;

      //      for (int i=0;i<100000;i++)
      //	ts.hasMatch(pat);

    }
}
