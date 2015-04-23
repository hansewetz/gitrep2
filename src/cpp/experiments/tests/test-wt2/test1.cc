// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015. All rights reserved.
#include "WtInputHandler.h"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#include <Wt/WApplication>
#pragma GCC diagnostic pop
#include <string>
#include <vector>
using namespace Wt;
using namespace std;

// NOTE!
vector<string>srclans{"en","sv","nb","el"};
vector<string>trglans{"en","sv","nb","el"};
size_t pollms{2000};

// create app.
WApplication *createApplication(const WEnvironment& env){
  return new WtInputHandler(pollms,srclans,trglans,env);
}
// main.
int main(int argc,char**argv){
return WRun(argc,argv,&createApplication);
}


