#include "xlate-jobs/TranslationJob.h"
#include <iostream>
using namespace std;
namespace xlate{

// ctors
TranslationJob::TranslationJob(shared_ptr<TranslateRequest>req):
  req_(req),translated_(req->segs().size()),translations_(req->segs().size()){
}
// print function
ostream&TranslationJob::print(ostream&os)const{
  return os<<"req: "<<*req_;
}
// print operator
ostream&operator<<(ostream&os,TranslationJob const&j){
  return j.print(os);
}
}
