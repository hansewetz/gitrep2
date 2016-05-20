// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015,2016. All rights reserved.

#ifndef __LANGUAGE_CODE_H__
#define __LANGUAGE_CODE_H__
#include <iosfwd>
#include <string>
#include <utility>
namespace xlate{

// language code
class LanguageCode{
public:
  // ctors, dtor
  LanguageCode(std::string const&lc);
  LanguageCode(LanguageCode const&)=default;
  LanguageCode(LanguageCode&&)=default;
  LanguageCode&operator=(LanguageCode const&)=default;
  LanguageCode&operator=(LanguageCode&&)=default;
  ~LanguageCode()=default;

  // getters
  std::string const&languageCode()const;

  // comparison operators
  bool operator==(LanguageCode const&lc)const{return lc_==lc.lc_;}
  bool operator<(LanguageCode const&lc)const{return lc<lc.lc_;}

  // print function
  std::ostream&print(std::ostream&os)const;
private:
  std::string lc_;
};
// print function
std::ostream&operator<<(std::ostream&os,LanguageCode const&lc);

// typedef for a language pair
using LanguagePair=std::pair<LanguageCode,LanguageCode>;

// helper creation functions
LanguagePair make_lanpair(std::string const&lan1,std::string const&lan2);

// make sure we get all relational operators
using namespace std::rel_ops;
}
#endif
