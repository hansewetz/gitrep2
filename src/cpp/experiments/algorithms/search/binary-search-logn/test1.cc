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
#include <stack>
#include <regex>

using namespace std;

// find start index of a value
int findStartIndex(vector<int>&v,int l,int r,int val){
  if(l>r)return l;
  int m=(l+r)/2;
  if(v[m]<val){
    return findStartIndex(v,m+1,r,val);
  }
  else{
    return findStartIndex(v,l,m-1,val);
  }
}
// find end index of a value
int findEndIndex(vector<int>&v,int l,int r,int val){
  if(l>r)return r;
  int m=(l+r)/2;
  if(v[m]<=val){
    return findEndIndex(v,m+1,r,val);
  }
  else{
    return findEndIndex(v,l,m-1,val);
  }
}

// test program
int main(){
  vector<int>v{5,3,9,9,9,9,9,9,5,6,7,8,2,3,4,5,6,6,5};
  sort(v.begin(),v.end());
  copy(v.begin(),v.end(),ostream_iterator<int>(cout," "));
  cout<<endl;
  
  // if lower bound > upper bound then number not found
  cout<<"start index for 9: "<<findStartIndex(v,0,v.size()-1,9)<<endl;
  cout<<"end   index for 9: "<<findEndIndex(v,0,v.size()-1,9)<<endl;
}
