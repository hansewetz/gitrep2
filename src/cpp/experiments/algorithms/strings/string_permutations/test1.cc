// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014. All rights reserved.

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


// mark functions - could be managed through a boolean array passed instead
void mark(char&c){c|=0x80;}
void unmark(char&c){c&=~0x80;}
bool marked(char c){return static_cast<bool>(c&0x80);}

// could be more efficient by passing remaining #of chars to stop recursion
void printStrPermAux(ostream&os,string&str,string&res){
  if(str.length()==res.length()){
    os<<res<<endl;
    return;
  }
  for(std::size_t i=0;i<str.length();++i){
    if(marked(str[i]))continue;
    res.push_back(str[i]);
    mark(str[i]);
    printStrPermAux(os,str,res);
    res.pop_back();
    unmark(str[i]);
  }
}
// interface to permutation function
void printStrPerm(ostream&os,string&str){
  string tmp;
  printStrPermAux(os,str,tmp);
}

int main(){
  string str{"1234"};
  printStrPerm(cout,str);
}
