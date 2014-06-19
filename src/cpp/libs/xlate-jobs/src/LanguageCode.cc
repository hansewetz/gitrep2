#include "xlate-jobs/LanguageCode.h"
#include <iostream>
using namespace std;
namespace xlate{

// ctors
LanguageCode::LanguageCode(string const&lc):lc_(lc){
}
// getters
string const&LanguageCode::languageCode()const{
  return lc_;
}
// print function
ostream&LanguageCode::print(ostream&os)const{
  return os<<lc_;
}
// print function
ostream&operator<<(ostream&os,LanguageCode const&lc){
  return lc.print(os);
}
// helper creation functions
LanguageCode make_lanpair(string const&lan){
  return LanguageCode(lan);
}
//create a pair of language codes
pair<LanguageCode,LanguageCode>make_lanpairs(string const&lan1,string const&lan2){
  return make_pair(make_lanpair(lan1),make_lanpair(lan2));
}
}
