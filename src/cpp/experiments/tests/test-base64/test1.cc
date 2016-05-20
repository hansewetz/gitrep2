// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015,2016. All rights reserved.

#include "base64.h"
#include <string>
#include <iostream>
using namespace std;

// main test program
int main(){
  string s{"Hello world\nAgain Hello world\nAgain Hello world\nAgain Hello world\nAgain Hello world\nAgain Hello world\nAgain Hello world\nAgain "};
  string s64{encode64(s)};
  
  cout<<s<<endl;
  cout<<s64<<endl;
  cout<<decode64(s64)<<endl;
}
