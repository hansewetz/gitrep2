// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015,2016. All rights reserved.

#include "xlate-jobs/TranslateRequest.h"
#include "utils/utility.h"
#include <iostream>
using namespace std;
namespace xlate{

// ctor
TranslateRequest::TranslateRequest(LanguagePair const&lanpair,initializer_list<string>segs):
  TranslateRequest(lanpair,vector<string>(segs)){
}
// ctor (re-use ctor taking vector of segments)
TranslateRequest::TranslateRequest(LanguagePair const&lanpair,vector<string>const&segs):
  lanpair_(lanpair),id_(TranslateRequestId()),segs_(segs){
}
// getters
TranslateRequestId const&TranslateRequest::id()const{
  return id_;
}
// get language pair
LanguagePair const&TranslateRequest::lanpair()const{
  return lanpair_;
}
// get request segments
vector<string>const&TranslateRequest::segs()const{
  return segs_;
}
// print function
ostream&TranslateRequest::print(ostream&os)const{
  os<<"id: "<<id_<<"srclan: "<<lanpair_.first<<", targetlan: "<<lanpair_.second<<endl;
  os<<"segs: "<<endl;
  for(auto const&s:segs_)os<<"\t"<<s<<endl;
  return os;
}
// print function
ostream&operator<<(ostream&os,TranslateRequest const&r){
  return r.print(os);
}
}
