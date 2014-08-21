#ifndef __TranslationRequestFactory_H__
#define __TranslationRequestFactory_H__
#include "xlate-jobs/LanguageCode.h"
#include <boost/filesystem.hpp>
#include <memory>
namespace xlate{

// forward decl
class TranslateRequest;

// ctor - make it an object so we can potentially configure for different languages
class TranslationRequestFactory{
public:
  // ctors,assign,dtor
  TranslationRequestFactory();
  TranslationRequestFactory(TranslationRequestFactory const&)=default;
  TranslationRequestFactory(TranslationRequestFactory&&)=default;
  TranslationRequestFactory&operator=(TranslationRequestFactory const&)=default;
  TranslationRequestFactory&operator=(TranslationRequestFactory&&)=default;
  ~TranslationRequestFactory()=default;

  // create a request from a segmented file - one segment per line
  std::shared_ptr<TranslateRequest>requestFromSegmentedFile(LanguagePair const&lp,boost::filesystem::path const&file)const;
};
}
#endif
