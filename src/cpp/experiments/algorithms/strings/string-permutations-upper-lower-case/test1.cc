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

using namespace std;

// ---------------------------------------------------------------------
// generate all permutations having every second letter upper/lower case
// #combination:
// - if nu == nl: 2*nu!
// - if nu > nl:  nu!*nl* (opposite if nl>nu
// ---------------------------------------------------------------------

// mark functions - could be managed through a boolean array passed instead
void mark(char&c){c|=0x80;}
void unmark(char&c){c&=~0x80;}
bool marked(char c){return static_cast<bool>(c&0x80);}

// could be more efficient by passing remaining #of chars to stop recursion
void printStrPermAux(ostream&os,string&str,string&res){
  // (1) check for solution
  if(str.length()==res.length()){
    os<<res<<endl;
    return;
  }
  // (1) get index of last character in result
  std::size_t resind=res.length()-1;
  
  // (2) check if there are no valid permutations
  std::size_t nu{0},nl{0};
  if(resind<0){
    // we only get here ones
    for(std::size_t i=0;i<str.length();++i){
      if(isupper(str[i]))++nu;
      else ++nl;
    }
    if(abs(nl-nu)>1)return;  // return if there are no permutations
  }
  // (3) loop over remaining characters
  for(std::size_t i=0;i<str.length();++i){
    if(marked(str[i]))continue;

    // check if we can prune
    if(resind<0){
      // (4) check if we can prune directly on first character
      if(nu>nl&&islower(str[i]))continue;
      if(nl>nu&&isupper(str[i]))continue;
    }
    else{
      // (5) next character must be upper/lower case based on previous character
      if(isupper(res[resind])){
        if(isupper(str[i]))continue;
      }
      else{
        if(islower(str[i]))continue;
      }
    }
    // (6) run back tracking algorithm
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
  string str{"HfFAaBbc"};
  printStrPerm(cout,str);
}
