#ifndef __TRANSLATE_JOB_H__
#define __TRANSLATE_JOB_H__
#include <string>
#include <iosfwd>
#include <memory>
#include <vector>
namespace xlate{

// forward decl
class TranslateRequest;
class TranslationTask;

// class representing a translation job (one or more segments together with data specifying what to do)
class TranslationJob{
public:
  // ctors, dtor (non-copyable, movable)
  TranslationJob(std::shared_ptr<TranslateRequest>);
  TranslationJob(TranslationJob const&)=delete;
  TranslationJob(TranslationJob&&)=default;
  TranslationJob&operator=(TranslationJob const&)=delete;
  TranslationJob&operator=(TranslationJob&&)=default;
  virtual~TranslationJob()=default;

  // book keeping functions
  //std::shared_ptr<TranslationTask>nextTask;
  

  // NOTE! Book keeping functions */
  //	- add translated segents
  //	- get segment for translation
  //	- get #of remaining segments

  // print function
  std::ostream&print(std::ostream&os)const;
private:
  std::shared_ptr<TranslateRequest>req_;
  std::vector<bool>translated_;
  std::vector<std::string>translations_;
};
std::ostream&operator<<(std::ostream&os,TranslationJob const&j);
}
#endif
