#ifndef __TRANSLATE_JOB_H__
#define __TRANSLATE_JOB_H__
#include <string>
#include <iosfwd>
#include <unordered_map>
namespace xlate{

// class representing a translation job (one or more segments together with data specifying what to do)
class TranslationJob{
public:
  // ctors, dtor (non-copyable, movable)
  TranslationJob(std::string const&id);
  TranslationJob(TranslationJob const&)=delete;
  TranslationJob&operator=(TranslationJob const&)=delete;
  TranslationJob&operator=(TranslationJob&&)=delete;
  virtual~TranslationJob()=default;

  // print function
  std::ostream&print(std::ostream&os)const;
private:
  std::string id_;
};
std::ostream&operator<<(std::ostream&os,TranslationJob const&j);
}
#endif
