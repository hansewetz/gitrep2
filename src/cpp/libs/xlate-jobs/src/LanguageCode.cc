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
}
