// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015. All rights reserved.

#include "xlate-tools/JobHandler.h"
#include <iostream>
using namespace std;
namespace xlate{

// ctor
JobHandler::JobHandler(std::shared_ptr<TranslationJob>job):job_(job){
}
// print operator
ostream&operator<<(ostream&os,JobHandler const&jh){
  jh.print(os);
  return os;
}
}
