#ifndef __TRANSLATE_REQUEST_H__
#define __TRANSLATE_REQUEST_H__
#include "xlate-jobs/LanguageCode.h"
#include "utils/Id.h"
#include <string>
#include <vector>
#include <iosfwd>
namespace xlate{

// request id
class TranslateRequest;
using TranslateRequestId=utils::Id<TranslateRequest,std::string,true>;

// request to traslate text
class TranslateRequest{
public:
  // ctors (movable, not copyable)
  TranslateRequest(LanguageCode const&slan,LanguageCode const&tlan,std::initializer_list<std::string>segs);
  TranslateRequest(LanguageCode const&slan,LanguageCode const&tlan,std::vector<std::string>const&segs);
  TranslateRequest(TranslateRequest const&)=delete;
  TranslateRequest(TranslateRequest&&)=default;
  TranslateRequest&operator=(TranslateRequest const&)=delete;
  TranslateRequest&operator=(TranslateRequest&&)=default;
  virtual~TranslateRequest()=default;

  // getters
  TranslateRequestId const&id()const;
  LanguageCode const&srcLan()const;
  LanguageCode const&targLan()const;
  std::vector<std::string>const&segs()const;

  // print function
  std::ostream&print(std::ostream&os)const;
private:
  TranslateRequestId id_;
  std::vector<std::string>segs_;
  LanguageCode slan_;
  LanguageCode tlan_;
};
// print function
std::ostream&operator<<(std::ostream&os,TranslateRequest const&);
}
#endif
