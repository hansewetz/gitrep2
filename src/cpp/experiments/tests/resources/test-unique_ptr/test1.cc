// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015,2016. All rights reserved.

#include <iostream>
#include <fstream>
#include <list>
#include <memory>
#include <functional>
using namespace std;

int main(){
  ifstream ifs("Makefile");
  std::function<void(ifstream*)>closer([](ifstream*ifs){cerr<<"closing ...";ifs->close();cerr<<" closed"<<endl;});
  unique_ptr<ifstream,std::function<void(ifstream*)>>uifs(&ifs,closer);
}

