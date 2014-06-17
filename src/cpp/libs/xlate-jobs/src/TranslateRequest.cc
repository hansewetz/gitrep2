#include "xlate-jobs/TranslateRequest.h"
#include "utils/utility.h"
#include <iostream>
using namespace std;
namespace xlate{

// ctor
TranslateRequest::TranslateRequest(string const&slan,string const&tlan,vector<string>const&segs):
  slan_(slan),tlan_(tlan),id_(utils::generateUuid()),segs_(segs){
}
// ctor
TranslateRequest::TranslateRequest(string const&slan,string const&tlan,initializer_list<string>segs):
  slan_(slan),tlan_(tlan),segs_(segs.begin(),segs.end()),id_(utils::generateUuid()){
}
// getters
string TranslateRequest::id()const{
  return id_;
}
// get source language
string TranslateRequest::srcLan()const{
  return slan_;
}
// get target language
string TranslateRequest::targLan()const{
  return tlan_;
}
// print function
ostream&TranslateRequest::print(ostream&os)const{
  os<<"id: "<<id_<<"srclan: "<<slan_<<", targetlan: "<<tlan_<<endl;
  os<<"segs "<<endl;
  for(auto const&s:segs_)os<<s<<endl;
  return os;
}
// print function
ostream&operator<<(ostream&os,TranslateRequest const&r){
  return r.print(os);
}
}
