#ifndef __TRANSLATE_REQUEST_H__
#define __TRANSLATE_REQUEST_H__
#include "xlate-jobs/LanguageCode.h"
#include "xlate-jobs/Identifiers.h"
#include <string>
#include <vector>
#include <iosfwd>
namespace xlate{

// request to traslate text
class TranslateRequest{
public:
  // ctors (movable, not copyable)
  TranslateRequest(LanguagePair const&lanpair,std::initializer_list<std::string>segs);
  TranslateRequest(LanguagePair const&lanpair,std::vector<std::string>const&segs);
  TranslateRequest(TranslateRequest const&)=delete;
  TranslateRequest(TranslateRequest&&)=default;
  TranslateRequest&operator=(TranslateRequest const&)=delete;
  TranslateRequest&operator=(TranslateRequest&&)=default;
  virtual~TranslateRequest()=default;

  // getters
  TranslateRequestId const&id()const;
  LanguagePair const&lanpair()const;
  std::vector<std::string>const&segs()const;

  // print function
  std::ostream&print(std::ostream&os)const;
private:
  TranslateRequestId id_;
  std::vector<std::string>segs_;
  LanguagePair lanpair_;
};
// print function
std::ostream&operator<<(std::ostream&os,TranslateRequest const&);
}
#endif
