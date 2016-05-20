// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015,2016. All rights reserved.

#include <iostream>
using namespace std;

// main test program
int main(){
  auto pf=[](auto t){cout<<t<<endl;}
  pf(15);
  pf("fifteen");
}
