#include <TreeSearch.h>
#include <iostream>
#include <bitset>
using namespace std;

// this program reads a list of these
typedef  unsigned long long patType;
//typedef  bitset<256> patType;



TreeSearch<patType,7> ts("pattern.dat");

int main (int argc , char **argv)
{
  patType pat=1;
  while (pat!=0)
    {
      cin>>hex>> pat;
      cout<<pat<<endl;
      cout<<ts.hasMatch(pat)<<endl;
      //      for (int i=0;i<100000;i++)
      //	ts.hasMatch(pat);

    }
}
