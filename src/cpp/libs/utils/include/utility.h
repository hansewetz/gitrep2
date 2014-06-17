#ifndef __UTILITY_H__
#define __UTILITY_H__
#include <cstdio>
#include <functional>
#include <string>
#include <sstream>
#include <stdexcept>
namespace utils{

// hash struct for classes having an 'asString() method
template<typename Key>
struct hash{
  std::size_t operator()(Key const&key)const{
    return std::hash<std::string>()(key.asString());
  }
};
// throw macro for exceptions
#define THROW_RUNTIME(x){\
  std::stringstream strm;\
  strm<<__FILE__":#"<<__LINE__<<": "<<x;\
    std::string s{strm.str()};\
    throw std::runtime_error(s);\
}
// generate a uuid
std::string generateUuid();
}
#endif
