#include "utils/fileUtils.h"
#include "utils/utility.h"
#include <memory>
#include <ostream>
#include <istream>
#include <fstream>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>
using namespace boost::filesystem;
using namespace std;
namespace io=boost::iostreams;
namespace utils{

// --- open a file for writing given: filename
std::shared_ptr<ostream>openWriteFileName(boost::filesystem::path const&filePath){
  std::shared_ptr<ostream>ret{std::shared_ptr<ostream>(new ofstream(filePath.string(),ofstream::trunc|ofstream::binary))};
  if(!*ret)THROW_RUNTIME("failed opening file: "<<filePath.string());
  return ret;
}
// --- open a file for reading given: filename
std::shared_ptr<istream>openReadFileName(boost::filesystem::path const&filePath){
  std::shared_ptr<istream>ret{std::shared_ptr<istream>(new ifstream(filePath.string(),ifstream::binary))};
  if(!*ret)THROW_RUNTIME("failed opening file: "<<filePath.string());
  return ret;
}
// get files ordered by timestamp in a map (with newest file first)
multimap<time_t,path>getTsOrderedFiles(path const&dirPath){
  // make sure path is a directory
  if(!is_directory(dirPath))throw logic_error(string("getTsOrderedFiles: dirPath: ")+dirPath.string()+" is not a directory");

  // need a map with key=time, value=filename
  typedef multimap<time_t,path>time_file_map_t;
  time_file_map_t time_file_map;

  // insert all files together with time as key into map
  directory_iterator dir_end_iter;
  for(directory_iterator it(dirPath);it!=dir_end_iter;++it){
    if(!is_regular_file(*it))continue;
    time_t time_stamp(last_write_time(*it));
    time_file_map.insert(time_file_map_t::value_type(time_stamp,*it));
  }
  return time_file_map;
}
// get the oldest file together - bool indicating if a file was found
pair<bool,path> getOldestFile(path const&dirPath){
  // get all pending message file ordered with newest first
  typedef multimap<time_t,path>time_file_map_t;
  time_file_map_t time_file_map(move(getTsOrderedFiles(dirPath)));
  if(time_file_map.size()==0)return make_pair(false,path());
  return make_pair(true,time_file_map.rbegin()->second);
}
// get vector of all files in a directory
vector<string>getFilesInDirectory(boost::filesystem::path const&dirPath){
  vector<string>ret;
  directory_iterator dir_end_iter;
  for(directory_iterator it(dirPath);it!=dir_end_iter;++it){
    if(!is_regular_file(*it))continue;
    ret.push_back((*it).path().filename().string());
  }
  return ret;
}
// check that directory exist and is a directory
bool isDirectory(boost::filesystem::path const&dirPath){
  return boost::filesystem::exists(dirPath)&&boost::filesystem::is_directory(dirPath);
}
// get an ostream from a file descriptor
shared_ptr<ostream>fdostream(int fd,bool close){
  return shared_ptr<ostream>(
    new ostream(new io::stream_buffer<io::file_descriptor_sink>(fd,close?io::close_handle:io::never_close_handle)));
}
// get an istream from a file descriptor
shared_ptr<istream>fdistream(int fd,bool close){
  return shared_ptr<istream>(
    new istream(new io::stream_buffer<io::file_descriptor_source>(fd,close?io::close_handle:io::never_close_handle)));
}
}
