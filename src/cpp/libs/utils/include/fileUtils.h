// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014. All rights reserved.

#ifndef __FILE_UTILS_H__
#define __FILE_UTILS_H__
#include <boost/filesystem.hpp>
#include <map>
#include <memory>
#include <vector>
#include <iosfwd>
namespace utils{

// --- open a file for writing given filename in binary mode
std::shared_ptr<std::ostream>openWriteFileName(boost::filesystem::path const&filePath);

// --- open a file for reading given filename in binary mode
std::shared_ptr<std::istream>openReadFileName(boost::filesystem::path const&filePath);

// get files ordered by timestamp in a map (with newest file first)
std::multimap<time_t,boost::filesystem::path>getTsOrderedFiles(boost::filesystem::path const&dirPath);

// get the oldest file from directory
std::pair<bool,boost::filesystem::path>getOldestFile(boost::filesystem::path const&dirPath);

// get vector of all files in a directory (without full path)
std::vector<std::string>getFilesInDirectory(boost::filesystem::path const&dirPath);

// check that directory exist and is a directory
bool isDirectory(boost::filesystem::path const&dirPath);

// get an ostream/istream from a file descriptor
std::shared_ptr<std::ostream>fdostream(int fd,bool close);
std::shared_ptr<std::istream>fdistream(int fd,bool close);
}
#endif
