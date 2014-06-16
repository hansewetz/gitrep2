#ifndef __TRANSLATE_REQUEST_H__
#define __TRANSLATE_REQUEST_H__
#include <string>
#include <vector>
#include <iosfwd>
namespace xlate{

// request to traslate text
class TranslateRequest{
public:
  // ctors (movable, not copyable)
  TranslateRequest(std::vector<std::string>const&segs);
  TranslateRequest(std::initializer_list<std::string>segs);
  TranslateRequest(TranslateRequest const&)=delete;
  TranslateRequest(TranslateRequest&&)=default;
  TranslateRequest&operator=(TranslateRequest&&)=default;
  virtual~TranslateRequest()=default;

  // getters
  std::string id()const;

  // print function
  std::ostream&print(std::ostream&os)const;
private:
  std::vector<std::string>segs_;
  std::string id_;
};
// print function
std::ostream&operator<<(std::ostream&os,TranslateRequest const&);
}
#endif
