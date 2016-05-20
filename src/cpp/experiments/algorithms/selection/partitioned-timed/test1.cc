// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015,2016. All rights reserved.

#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <boost/timer/timer.hpp>
using namespace std;

int partition(vector<int>&v,int l,int u){
  int m=l;
  int t=v[m];
  for(int i=l+1;i<=u;++i){
    if(v[i]<t)swap(v[++m],v[i]);
  }
  swap(v[l],v[m]);
  return m;
}
int main(){
  // setup random generator
  const int nmax=100000000;
  std::random_device rd;
  std::default_random_engine e1(rd());
  std::uniform_int_distribution<int> ud(1, nmax);

  // create vector
  vector<int>v;
  v.reserve(nmax);
  for(int i=0;i<nmax;++i){
    v.push_back(ud(e1));
  }
  // partition and measure time
  int p;
  {
    boost::timer::auto_cpu_timer t;
    p=partition(v,0,v.size()-1);
  }
  //print partition element
  cerr<<endl<<"p: "<<p<<endl;
}
