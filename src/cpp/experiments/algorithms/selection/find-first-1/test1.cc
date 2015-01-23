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
#include <cstring>
#include <array>
#include <stack>
#include <regex>

using namespace std;

int findFirstOneAux(vector<int>&v,int l,int u){
  if(l>u)return l;
  int m=(l+u)/2;
  if(v[m]==1){
    return findFirstOneAux(v,l,m-1);
  }
  else{
    return findFirstOneAux(v,m+1,u);
  }
}
int findFirstOne(vector<int>&v){
  return findFirstOneAux(v,0,v.size()-1);
}

// test program
int main(){
  vector<int>v{0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
  cout<<"first 1: "<<findFirstOne(v)<<endl;
}
