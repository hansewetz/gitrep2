#include "xlate-jobs/TranslationJob.h"
#include <iostream>
using namespace std;
namespace xlate{

// ctors
TranslationJob::TranslationJob(string const&id):id_(id){
  // NOTE! Not yet done
}
// print function
ostream&TranslationJob::print(ostream&os)const{
  return os<<"id: "<<id_;
}
// print operator
ostream&operator<<(ostream&os,TranslationJob const&j){
  return j.print(os);
}
}
