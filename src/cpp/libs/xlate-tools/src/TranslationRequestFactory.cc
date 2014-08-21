#include "xlate-tools/TranslationRequestFactory.h"
#include "xlate-jobs/TranslateRequest.h"
#include "utils/fileUtils.h"
#include <boost/filesystem.hpp>
#include <vector>
#include <fstream>
#include <string>
using namespace std;
namespace xlate{

// ctor - from file
TranslationRequestFactory::TranslationRequestFactory(){
}
// create a request from a segmented file - one segment per line
std::shared_ptr<TranslateRequest>TranslationRequestFactory::requestFromSegmentedFile(LanguagePair const&lp,boost::filesystem::path const&file)const{
  // open file and read it line by line, insert each line in a vector and create a request
  std::shared_ptr<istream>is{utils::openReadFileName(file)};
  vector<string>lines;
  string line;
  while(getline(*is,line))lines.push_back(line);
  return std::make_shared<TranslateRequest>(lp,lines);
}
}
