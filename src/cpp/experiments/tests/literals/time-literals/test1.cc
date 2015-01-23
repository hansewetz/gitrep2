// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015. All rights reserved.

// see Stroustrup p.558

#include <boost/lexical_cast.hpp>
#include <iostream>
#include <chrono>
using namespace std;

// duration literal
// (taking only a char*, the literal can be specified using digits)
chrono::duration<int>operator"" _sec(char const*s){
  size_t n=boost::lexical_cast<size_t>(s);
  return chrono::duration<int>(n);
}

// test main program
int main(){
  auto nsec(17_sec);
  cout<<"17 seconds: "<<nsec.count()<<endl;
}
