#ifndef __TRANSLATE_JOB_H__
#define __TRANSLATE_JOB_H__
#include "utils/Id.h"
#include <string>
#include <iosfwd>
#include <memory>
#include <vector>
namespace xlate{

// forward decl
class TranslateRequest;
class TranslationTask;
class TranslationJob;

// job id
using TranslationJobId=utils::Id<TranslationJob,std::string,true>;

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
  TranslationJobId const&id()const;
  

  // NOTE! Book keeping functions */
  //	- add translated segents
  //	- get segment for translation
  //	- get #of remaining segments

  // print function
  std::ostream&print(std::ostream&os)const;
private:
  TranslationJobId id_;
  std::shared_ptr<TranslateRequest>req_;
  std::vector<bool>translated_;
  std::vector<std::string>translations_;
};
std::ostream&operator<<(std::ostream&os,TranslationJob const&j);
}
#endif
