// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015,2016. All rights reserved.

#ifndef _LOG_UTILS_H_
#define _LOG_UTILS_H_
#include <string>
namespace utils{

// log levels
enum class UtilsLogLevel{TRACE=0,DEBUG=1,NORMAL=2};

// initialize logging
void initBoostFileLogging(UtilsLogLevel level,std::string const&path,std::string const&basename,std::size_t maxsize,std::size_t maxfiles);
void initBoostLogging(UtilsLogLevel level);
void initBoostLogging(bool debug);
}
#endif
