#ifndef __LANGUAGE_CODE_H__
#define __LANGUAGE_CODE_H__
#include <iosfwd>
#include <string>
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

  // print function
  std::ostream&print(std::ostream&os)const;
private:
  std::string lc_;
};
// print function
std::ostream&operator<<(std::ostream&os,LanguageCode const&lc);
}
#endif
