#include "xlate-jobs/TranslateRequest.h"
#include <iostream>
using namespace std;
namespace xlate{

// ctor
TranslateRequest::TranslateRequest(vector<string>const&segs):segs_(segs){
}
// ctor
TranslateRequest::TranslateRequest(initializer_list<string>segs):segs_(segs.begin(),segs.end()){
}
// print function
ostream&TranslateRequest::print(ostream&os)const{
  for(auto const&s:segs_)os<<s<<endl;
  return os;
}
// print function
ostream&operator<<(ostream&os,TranslateRequest const&r){
  return r.print(os);
}
}
