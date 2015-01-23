// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015. All rights reserved.

#include <iostream>
#include <stdexcept>
using namespace std;

// base function without any arguments
void print(const char*s){
  while(*s){
    if(*s=='%'){
      if(*(s+1)=='%'){
        ++s;
      }
      else{
        throw runtime_error("missing parameters");
      }
    }
    cout<<*s++;
  }
}
// template print with arguments 
template<typename T, typename... Args>
void print(const char*s,T value, Args... args){
  while(*s){
    if(*s=='%'){
      if(*(s+1)=='%')++s;
      else {
        std::cout<<value;
        print(s+1,args...); // call even when *s == 0 to detect extra arguments
        return;
      }
    }
    std::cout<<*s++;
  }
  throw logic_error("extra arguments provided to print");
}
// main.
int main(int argc,char**argv){
  print("Hello % world %\n","World",1174);
}

