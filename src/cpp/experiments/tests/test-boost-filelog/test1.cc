// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015. All rights reserved.

#include "logUtils.h"
#include <boost/log/trivial.hpp>
using namespace std;

int main(){
  // initialise file logging
  // (rotate at 200 bytes, keep a maximum of 200*3 bytes in log files before deleting files)
  initBoostFileLogging(utils::UtilsLogLevel::NORMAL,".","test",200,3);

  // try a few log statements
  for(int i=0;i<1000;++i){
    BOOST_LOG_TRIVIAL(debug)<<"not visible";
    BOOST_LOG_TRIVIAL(info)<<"visible";
    BOOST_LOG_TRIVIAL(error)<<"visible";
  }
}
