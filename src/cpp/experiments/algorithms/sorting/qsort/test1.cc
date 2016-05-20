// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015,2016. All rights reserved.

#include <iostream>
#include <algorithm>
#include <vector>
#include <iterator>
#include <random>
#include <boost/timer/timer.hpp>

using namespace std;

void Qsort(vector<int>&v,int l,int u){
  // stop recursion
  if(l>=u)return;

  // partition
  int m=l;
  int t=v[m];
  for(int i=l+1;i<=u;++i){
    if(v[i]<t)swap(v[++m],v[i]);
  }
  swap(v[l],v[m]);

  // sort
  Qsort(v,l,m-1);
  Qsort(v,m+1,u);
}
int main(){
  const int NMAX=100000000;
  std::random_device rd;
  std::default_random_engine e1(rd());
  std::uniform_int_distribution<int> dist(1,NMAX);

  vector<int>v;
  v.reserve(NMAX);
  for(int i=0;i<NMAX;++i)v.push_back(dist(e1));
/*
  {
    boost::timer::auto_cpu_timer t;
    Qsort(v,0,v.size()-1);
  }
*/
  copy(v.begin(),v.end(),ostream_iterator<int>(cout,"\n"));
}
