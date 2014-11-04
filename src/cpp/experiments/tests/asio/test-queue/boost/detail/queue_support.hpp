#ifndef __DIRQUEUE_SUPPPORT_H__
#define __DIRQUEUE_SUPPPORT_H__

#include <algorithm>
#include <string>
#include <map>
#include <list>
#include <utility>
#include <fstream>
#include <memory>
#include <ostream>
#include <istream>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/sync/named_condition.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>

namespace boost{
namespace asio{
namespace detail{
namespace queue_support{

namespace fs=boost::filesystem;
namespace ipc=boost::interprocess;

namespace{
// get all filenames in time sorted order in a dircetory
std::list<fs::path>getTsOrderedFiles(fs::path const&dir){
  // need a map with key=time, value=filename
  typedef std::multimap<time_t,fs::path>time_file_map_t;
  time_file_map_t time_file_map;

  // insert all files together with time as key into map
  fs::directory_iterator dir_end_iter;
  for(fs::directory_iterator it(dir);it!=dir_end_iter;++it){
    if(!is_regular_file(*it))continue;
    time_t time_stamp(last_write_time(*it));
    time_file_map.insert(time_file_map_t::value_type(time_stamp,*it));
  }
  // create return list and return
  std::list<fs::path>ret;
  for(auto const&f:time_file_map)ret.push_back(f.second);
  return ret;
}
// remove lock variables for queue
// (name of lock variables are computed from the path to the queue directory)
void removeLockVariables(std::string const&name){
  ipc::named_mutex::remove(name.c_str());
  ipc::named_condition::remove(name.c_str());
}
// helper function for serialising an object
// (lock must be held when calling this function)
template<typename T,typename SERIAL>
void write(T const&t,fs::path const&dir,SERIAL serial){
  // create a unique filename, open file for writing and serialise object to file (user defined function)
  // (serialization function is a user supplied function - see ctor)
  std::string const id{boost::lexical_cast<std::string>(boost::uuids::random_generator()())};
  fs::path fullpath{dir/id};
  std::ofstream os{fullpath.string(),std::ofstream::binary};
  if(!os)throw std::runtime_error(std::string("asio::detail::dirqueue_support::::write: could not open file: ")+fullpath.string());
  serial(os,t);
  os.close();
}
// helper function for deserialising an object
// (lock must be held when calling this function)
template<typename T,typename DESER>
T read(fs::path const&fullpath,DESER deser){
  // open input stream, deserialize stream into an object and remove file
  // (deserialization function is a user supplied function - see ctor)
  std::ifstream is{fullpath.string(),std::ifstream::binary};
  if(!is)throw std::runtime_error(std::string("asio::detail::dirqueue_support::read: could not open file: ")+fullpath.string());
  T ret{deser(is)};
  is.close();
  std::remove(fullpath.string().c_str());
  return ret;
}
// select from a single fd for reading - when triggered an istream is returned, or in case of a tno, an error code is returns
// (note: thi scall is blocking until either the timer pops or an event occurs in the read fd)
// (return false if timeout, else tru ena dfd can be read form)
std::shared_ptr<std::istream>selectFd(int fd,std::size_t ms,boost::system::error_code&ec){
  // set error code to OK
  ec=boost::system::error_code{};

  // setup for select statement
  fd_set input;
  fd_set output;
  FD_ZERO(&input);
  FD_ZERO(&output);
  FD_SET(fd,&input);
  int maxfd=fd;
  
  // setup for timeout
  struct timeval tmo;
  if(ms>0){
    tmo.tv_sec=ms/1000;
    tmo.tv_usec=(ms%1000)*1000;
  }
  // block on select - timeout if configured
  assert(maxfd!=-1);
  int n=select(++maxfd,&input,&output,NULL,(ms>0)?&tmo:NULL);

  // check result of select
  if(n<0)throw std::runtime_error(std::string("select() failed, errno: ")+boost::lexical_cast<std::string>(errno));
  if(n==0){
    ec=boost::asio::error::timed_out;
    return std::make_shared<std::istream>(nullptr);
  }
  // check for tmo or data
  // (notice that reading/writing 0 bytes from a pipe means that either the reading/writing process died)
  if(FD_ISSET(fd,&input)){
    return std::shared_ptr<std::istream>(
      new std::istream(new boost::iostreams::stream_buffer<boost::iostreams::file_descriptor_source>(fd,boost::iostreams::never_close_handle)));
  }
  // unknown error

  ec=boost::asio::error::operation_aborted;
  return std::make_shared<std::istream>(nullptr);
}
}
}
}
}
}
#endif
