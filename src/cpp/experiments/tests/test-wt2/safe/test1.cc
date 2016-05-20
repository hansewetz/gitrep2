// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015,2016. All rights reserved.

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#include <Wt/WApplication>
#pragma GCC diagnostic pop
using namespace Wt;
using namespace std;

// create app.
WApplication *createApplication(const WEnvironment& env){
  return new WtInputHandler(env);
}
// main.
int main(int argc,char**argv){
return WRun(argc,argv,&createApplication);
}


