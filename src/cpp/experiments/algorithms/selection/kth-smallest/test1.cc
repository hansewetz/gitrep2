// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015,2016. All rights reserved.

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
#include <cstring>
#include <array>

using namespace std;

int partition(vector<int>&v,int l,int u){
  int m=l;
  int t=v[l];
  for(int i=l;i<=u;++i){
    if(v[i]<t)swap(v[++m],v[i]);
  }
  swap(v[m],v[l]);
  return m;
}
int kthsmallest(vector<int>&v,int k){
  k-=1;
  if(k<0||k>=static_cast<int>(v.size()))throw runtime_error("kth element outside range of vector");
  int l=0;
  int u=v.size()-1;
  while(l<=u){
    int p=partition(v,l,u);
    if(k==p)return v[p];
    if(k<p)u=p-1;
    else   l=p+1;
  }
  // will never reach here
  return -1;
}

int main(){
  vector<int>v{5,2,8,9,1,3};
  for(int k=1;k<=static_cast<int>(v.size());++k){
    cerr<<k<<"th smallest number (counting from 1): "<<kthsmallest(v,k)<<endl;
  }
  sort(v.begin(),v.end());
  copy(v.begin(),v.end(),ostream_iterator<int>(cout," "));
}
