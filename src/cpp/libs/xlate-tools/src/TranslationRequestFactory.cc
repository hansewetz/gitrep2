#include "xlate-tools/TranslationRequestFactory.h"
#include "xlate-jobs/TranslateRequest.h"
#include <boost/filesystem.hpp>
#include <vector>
using namespace std;
namespace xlate{

// ctor - from file
TranslationRequestFactory::TranslationRequestFactory(){
}
// create a request from a segmented file - one segment per line
std::shared_ptr<TranslateRequest>TranslationRequestFactory::requestFromSegmentedFile(LanguagePair const&lp,boost::filesystem::path const&file)const{
  // NOTE! test code
  vector<string>segs{"Hello World","Goodby World","Last message","24 hours", "many movies"};
  return std::make_shared<TranslateRequest>(lp,segs);
}
}
