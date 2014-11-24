// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014. All rights reserved.

#include <iostream>
#include <string>
#include <exception>
#include <stdexcept>
#include <fstream>
using namespace std;

int main(){
  try{
    ifstream is("Makefile");
    if(!is)throw runtime_error("could not open file");
    string line;
    while(getline(is,line)){
      cerr<<line<<endl;
    }
  }
  catch(exception&e){
    cerr<<"caught exception: \""<<e.what()<<"\""<<endl;
    exit(1);
  }
}

