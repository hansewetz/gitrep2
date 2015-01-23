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
#include <array>

using namespace std;

// find first non-repeating cahracter in string (assuming all chars are < 128)
int find1stnonrepeat(string const&str){
  map<char,int>m;
  for(int i=0;i<str[i];++i)++m[str[i]];
  for(int i=0;i<str[i];++i){
    if(m[str[i]]==1)return str[i];
  }
  return -1;
}
int main(){
  string const str="Hello world";
  int c=find1stnonrepeat(str);
  if(c==-1){
    cerr<<"all charactres repeated"<<endl;
  }
  else{
    cerr<<"first non-repeating character: "<<static_cast<char>(c)<<endl;
  }
}
