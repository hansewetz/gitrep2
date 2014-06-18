#include "xlate-jobs/TranslateRequest.h"
#include "utils/utility.h"
#include <iostream>
using namespace std;
namespace xlate{

// ctor
TranslateRequest::TranslateRequest(LanguageCode const&slan,LanguageCode const&tlan,initializer_list<string>segs):
  TranslateRequest(slan,tlan,vector<string>(segs)){
}
// ctor (re-use ctor taking vector of segments)
TranslateRequest::TranslateRequest(LanguageCode const&slan,LanguageCode const&tlan,vector<string>const&segs):
  slan_(slan),tlan_(tlan),id_(TranslateRequestId()),segs_(segs){
}
// getters
TranslateRequestId const&TranslateRequest::id()const{
  return id_;
}
// get source language
LanguageCode const&TranslateRequest::srcLan()const{
  return slan_;
}
// get target language
LanguageCode const&TranslateRequest::targLan()const{
  return tlan_;
}
// get request segments
vector<string>const&TranslateRequest::segs()const{
  return segs_;
}
// print function
ostream&TranslateRequest::print(ostream&os)const{
  os<<"id: "<<id_<<"srclan: "<<slan_<<", targetlan: "<<tlan_<<endl;
  os<<"segs: "<<endl;
  for(auto const&s:segs_)os<<"\t"<<s<<endl;
  return os;
}
// print function
ostream&operator<<(ostream&os,TranslateRequest const&r){
  return r.print(os);
}
}
