// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015,2016. All rights reserved.

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
  LanguagePair lanpair_;
  TranslateRequestId id_;
  std::vector<std::string>segs_;
};
// print function
std::ostream&operator<<(std::ostream&os,TranslateRequest const&);
}
#endif
