// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015,2016. All rights reserved.

#include "xlate-jobs/EngineEnv.h"
#include <iostream>
using namespace std;
namespace fs=boost::filesystem;
namespace xlate{

// ctot
EngineEnv::EngineEnv(fs::path const&exedir,fs::path const&progpath,string const&ename,size_t tmoMs,size_t startTmoMs):
    exedir_(exedir),progpath_(progpath),ename_(ename),tmoMs_(tmoMs),startTmoMs_(startTmoMs){
}

// getters
fs::path const&EngineEnv::exedir()const{return exedir_;}
fs::path const&EngineEnv::progpath()const{return progpath_;}
string const&EngineEnv::ename()const{return ename_;}
std::size_t EngineEnv::tmoMs()const{return tmoMs_;}
std::size_t EngineEnv::startTmoMs()const{return startTmoMs_;}

// print operator
ostream&operator<<(ostream&os,EngineEnv const&env){
  return os<<"exedir: "<<env.exedir().string()<<", engine-name: "<<env.ename()<<", progpath: "<<env.progpath().string()<<
             ", tmp (ms): "<<env.tmoMs()<<", startTmoMs: "<<env.startTmoMs();
}
}
