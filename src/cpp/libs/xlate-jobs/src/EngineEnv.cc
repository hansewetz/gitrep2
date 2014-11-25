#include "xlate-jobs/EngineEnv.h"
#include <iostream>
using namespace std;
namespace fs=boost::filesystem;
namespace xlate{

// ctot
EngineEnv::EngineEnv(fs::path const&basepath,fs::path const&ename,fs::path const&tmpdir,fs::path const&trainenv):
    basepath_(basepath),ename_(ename),tmpdir_(tmpdir),trainenv_(trainenv){
}
// getters
fs::path const&EngineEnv::basepath()const{return basepath_;}
fs::path const&EngineEnv::ename()const{return ename_;}
fs::path const&EngineEnv::tmpdir()const{return tmpdir_;}
fs::path const&EngineEnv::trainenv()const{return trainenv_;}

// print operator
ostream&operator<<(ostream&os,EngineEnv const&env){
  return os<<"basepath: "<<env.basepath().string()<<", engine-name: "<<env.ename().string()<<", tmpdir: "<<env.tmpdir().string()<<", trainenv: "<<env.trainenv().string();
}
}

