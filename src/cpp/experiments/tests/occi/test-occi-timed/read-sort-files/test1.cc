// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015,2016. All rights reserved.

#include <iostream>
#include <fstream>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/timer/timer.hpp>

using namespace std;

// test program
int main(){
  ifstream is("junk.txt");
  ofstream os("junk-out.txt");
  string line;
  line.reserve(10000);
  int count=0;
  vector<string>matids;
  matids.reserve(21000000);
  cout<<"reading"<<endl;
  {
    boost::timer::auto_cpu_timer t;
    while(getline(is,line)){
      matids.push_back(line);
      ++count;
    }
  }
  cout<<"sorting"<<endl;
  {
    boost::timer::auto_cpu_timer t;
    sort(matids.begin(),matids.end(),[](string const&s1,string const&s2)->bool{return memcmp(s1.data(),s2.data(),6)>0;});
  }
  cout<<"writing"<<endl;
  {
    for(auto it=matids.begin();it!=matids.end();++it){
      os<<*it<<endl;
    }
  }
  cout<<"count: "<<count<<endl;
}
