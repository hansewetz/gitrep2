// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015,2016. All rights reserved.

#ifndef __LOG_UTILS_H__
#define __LOG_UTILS_H__
#include <string>
namespace utils{

// initialize logging
void initBoostFileLogging(bool verbose,std::string const&path,std::string const&basename,std::size_t maxsize);
void initBoostFileLogging(bool verbose);
}
#endif
