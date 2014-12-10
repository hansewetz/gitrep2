// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014. All rights reserved.

#include "NorwegianTmxExtractor.h"
#include <iostream>
#include <boost/log/trivial.hpp>

using namespace std;
using namespace mtdata;

// main test program
int main(int argc, char **argv) {
  if (argc!=2){
    fprintf(stderr,"usage: %s <xml-file>\n",argv[0]);
    return(1);
  }
  // create NorwegianTmxExtractor object
  BOOST_LOG_TRIVIAL(debug)<<"Starting ...";
  NorwegianTmxExtractor norExtract{argv[1]};

  // print dbid
  cout<<"dbid: \""<<norExtract.dbid()<<"\""<<endl<<endl;

  // print segments
  for(auto const&s:norExtract.segs())cout<<s<<endl;
}
