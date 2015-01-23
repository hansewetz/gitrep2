// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015. All rights reserved.

#include <iostream>
#include <exception>
#include <fstream>
#include <list>
#include <set>
#include <thread>
#include <mutex>
#include <iterator>
#include <vector>
#include <future>
#include <functional>
#include <algorithm>
#include <random>
#include <chrono>
#include <map>
#include <utility>
#include <unistd.h>
#include <type_traits>
#include <queue>
#include <chrono>
#include <ratio>
#include <cassert>

using namespace std;


// notice: m belongs to lower partition
void merge(vector<int>&v,vector<int>&tmp,int l,int m,int u){
  int l1=l,u1=m,l2=m+1,u2=u,isrt=l;

  while(l1<=u1&&l2<=u2){
    if(v[l1]<v[l2])tmp[isrt++]=v[l1++];
    else tmp[isrt++]=v[l2++];
  }
  while(l1<=u1)tmp[isrt++]=v[l1++];
  while(l2<=u2)tmp[isrt++]=v[l2++];
  for(int i=l;i<=u;++i)v[i]=tmp[i];
}
void msortAux(vector<int>&v,vector<int>&tmp,int l,int u){
  if(l>=u)return;
  int m=(l+u)/2;
  msortAux(v,tmp,l,m);
  msortAux(v,tmp,m+1,u);
  merge(v,tmp,l,m,u);
}
void msort(vector<int>&v){
  vector<int>tmp(v.size());
  msortAux(v,tmp,0,v.size()-1);
}

int main(){
  // construct a random generator
  std::minstd_rand0 engine;
  std::uniform_int_distribution<int>distr(5,30);
  std::function<int()>gen=std::bind(distr,engine);

  // fill a vector with random numbers
  vector<int>v;
  for(int i=0;i<15;++i)v.push_back(gen());

  // print vector
  copy(v.begin(),v.end(),ostream_iterator<int>(cout," "));
  cerr<<endl;
  msort(v);
  copy(v.begin(),v.end(),ostream_iterator<int>(cout," "));
  cerr<<endl;
}
