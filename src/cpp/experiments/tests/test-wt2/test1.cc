// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015,2016. All rights reserved.

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#include <Wt/WApplication>
#pragma GCC diagnostic pop
#include <string>
#include <vector>
#include <thread>

using namespace Wt;
using namespace std;

// NOTE!
vector<pair<string,string>>lanpairs{{"en","sv"},{"sv","en"},{"en","nb"},{"en","el"}};
size_t pollms{2000};

// create app.
WApplication *createApplication(const WEnvironment& env){
  return new WtInputHandler(pollms,lanpairs,env);
}
// main.
int main(int argc,char**argv){
  std::thread thr([&](){WRun(argc,argv,&createApplication);});
  thr.join();
}
