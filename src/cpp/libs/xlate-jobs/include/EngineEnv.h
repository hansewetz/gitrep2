// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014. All rights reserved.

#ifndef __ENGINE_ENV_H__
#define __ENGINE_ENV_H__
#include <iosfwd>
#include <boost/filesystem.hpp>
namespace fs=boost::filesystem;
namespace xlate{

// class specifying the environment in which an engine run in
class EngineEnv{
public:
  // ctors,assign,dtor
  EngineEnv(fs::path const&basepath,fs::path const&ename,fs::path const&tmpdir,fs::path const&trainenv);
  EngineEnv(EngineEnv const&)=default;
  EngineEnv(EngineEnv&&)=default;
  EngineEnv&operator=(EngineEnv const&)=default;
  EngineEnv&operator=(EngineEnv&&)=default;
  ~EngineEnv()=default;

  // getters
  fs::path const&basepath()const;
  fs::path const&ename()const;
  fs::path const&tmpdir()const;
  fs::path const&trainenv()const;
private:
  fs::path basepath_;        // path to directory under which language engines are stored
  fs::path ename_;           // name of engine executable
  fs::path tmpdir_;          // temporary directory
  fs::path trainenv_;        // engine needs TRAININGENVIRONMENT env variable
  //                         // add command line options
};
// print operator
std::ostream&operator<<(std::ostream&os,EngineEnv const&);
}
#endif
