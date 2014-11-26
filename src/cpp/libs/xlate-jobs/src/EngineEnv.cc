// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014. All rights reserved.

#include "xlate-jobs/EngineEnv.h"
#include <iostream>
using namespace std;
namespace fs=boost::filesystem;
namespace xlate{

// ctot
EngineEnv::EngineEnv(fs::path const&exedir,fs::path const&progpath,std::string const&ename):
    exedir_(exedir),progpath_(progpath),ename_(ename){
}

// getters
fs::path const&EngineEnv::exedir()const{return exedir_;}
fs::path const&EngineEnv::progpath()const{return progpath_;}
std::string const&EngineEnv::ename()const{return ename_;}

// print operator
ostream&operator<<(ostream&os,EngineEnv const&env){
  return os<<"exedir: "<<env.exedir().string()<<", engine-name: "<<env.ename()<<", progpath: "<<env.progpath().string();
}
}

