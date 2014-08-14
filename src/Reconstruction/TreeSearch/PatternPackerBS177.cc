#include <TreeSearch.h>
#include <iostream>
#include<vector>
#include<map>
#include <cmath>
#include<fstream>
#include<bitset>
using namespace std;


#define levels 9 //outer

// this program reads a list of these

typedef  bitset<177> patType; //for outer

// and compiles a pattern

typedef TreeSearch<patType,levels>::entry entry;


// use this for bitsets
struct BitsetLessThan
    : public std::binary_function<patType, patType, bool>
{
    bool operator() (patType const& n1, patType const& n2) const
    {
      for (unsigned int i=0;i<n1.size();i++)
	{
	  if (n1[i]<n2[i]) return true;
	  if (n1[i]>n2[i]) return false;
	}
      return false;
    }
};



map<patType,vector<patType>,BitsetLessThan > tree[levels];// outer

patType masks[levels];

vector<entry>entries;

void writekey(int lv,patType pat)
{
  int pos;
  if (lv==0)
    {
      entry e;
      e.pattern=tree[0][pat][0];
      //      e.level=0;
      e.skipto=0;
      cout<<"Writing "<<e.pattern<<" "<<e.skipto<<endl;
      entries.push_back(e);
      return;
    }
  for (unsigned int i=0;i<tree[lv][pat].size();i++)
    {
      entry e;
      e.pattern=tree[lv][pat][i];
      //      e.level=lv;
      entries.push_back(e);
      pos=entries.size()-1;
      writekey(lv-1,e.pattern);
      entries[pos].skipto=entries.size();
    }
  entries[pos].skipto=0;
}

int main()
{
  patType pat;
  // first, read all patterns (HEX CODE!)
  while (cin.good())
    {
      cin>>hex>> pat;
      if (!cin.good()) break;
      cout<<pat<<endl;
      tree[0][pat].push_back(pat);
    }

  // create masks
  patType mask=1;
  cout<<hex<<showbase<<"Masks:"<<endl;

  for (int i=levels-1;i>=0;i--)
    {
      masks[i]=mask;
      cout<<i<<" "<<mask<<endl;
      mask|=mask<<((int) pow(2.0,i-1));
    }
  unsigned int shift=1;

  for (int i=0;i<levels-1;i++)
    {
      for (map<patType,vector<patType> >::iterator iter=tree[i].begin() ;iter!=tree[i].end();iter++)
	{
	  patType b=iter->first;
	  patType a=(b | (b>>shift)) & masks[i+1];
	  cout<< i<< " "<<shift<<" B -> A: "<<b<<" -> "<<a<<endl;
	  tree[i+1][a].push_back(b);
	  //      tree[i+1].insert(pair<patType,patType>(a,b))
	}
      shift*=2;
      cout<<endl;
    }

  //now we have to assemble the entry list.
  cout<<"Starting writekey"<<endl;
  writekey(levels-1,1);
  cout<<dec<<endl;
  for (unsigned int i=0;i<entries.size();i++)
    cout<<i<< " "<<hex<<entries[i].pattern<<dec<<" "<<entries[i].skipto<<endl;
  //dumping 
  ofstream off("pattern.dat");

  for (unsigned int i=0;i<entries.size();i++)
    off.write((const char *)&entries[i],sizeof(entry));
  off.close();
  

}
