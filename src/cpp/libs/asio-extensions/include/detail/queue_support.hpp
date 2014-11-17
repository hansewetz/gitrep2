#ifndef __QUEUE_SUPPPORT_H__
#define __QUEUE_SUPPPORT_H__

#include <algorithm>
#include <string>
#include <map>
#include <list>
#include <iostream>
#include <utility>
#include <fstream>
#include <memory>
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
namespace io=boost::iostreams;

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
// get an ostream from a file descriptor
std::shared_ptr<std::ostream>makefd_ostream(int fd,bool close){
  return std::shared_ptr<std::ostream>(
    new std::ostream(new io::stream_buffer<io::file_descriptor_sink>(fd,close?io::close_handle:io::never_close_handle)));
}
// get an istream from a file descriptor
std::shared_ptr<std::istream>makefd_istream(int fd,bool close){
  return std::shared_ptr<std::istream>(
    new std::istream(new io::stream_buffer<io::file_descriptor_source>(fd,close?io::close_handle:io::never_close_handle)));
}
}
}
}
}
}
#endif
