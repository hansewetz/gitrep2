// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015. All rights reserved.

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
  EngineEnv(fs::path const&exedir,fs::path const&progpath,std::string const&ename,std::size_t tmoMs,std::size_t startTmoMs);
  EngineEnv(EngineEnv const&)=default;
  EngineEnv(EngineEnv&&)=default;
  EngineEnv&operator=(EngineEnv const&)=default;
  EngineEnv&operator=(EngineEnv&&)=default;
  ~EngineEnv()=default;

  // getters
  fs::path const&exedir()const;
  fs::path const&progpath()const;
  std::string const&ename()const;
  std::size_t tmoMs()const;
  std::size_t startTmoMs()const;
private:
  fs::path exedir_;               // directory t execute from
  fs::path progpath_;             // path to directory under which language engines are stored
  std::string ename_;             // name of engine executable
  std::size_t tmoMs_;             // staeady state timeout for a segment
  std::size_t startTmoMs_;        // start time for an engine
};
// print operator
std::ostream&operator<<(std::ostream&os,EngineEnv const&);
}
#endif
