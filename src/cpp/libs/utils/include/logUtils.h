#ifndef __LOG_UTILS_H__
#define __LOG_UTILS_H__
#include <string>
namespace utils{

// initialize logging
void initBoostFileLogging(bool verbose,std::string const&path,std::string const&basename,std::size_t maxsize);
void initBoostFileLogging(bool verbose);
}
#endif
