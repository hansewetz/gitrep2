#include "xlate-jobs/TranslationJob.h"
#include "xlate-jobs/TranslateRequest.h"
#include "utils/Id.h"
#include <iostream>
using namespace std;
namespace xlate{

// ctors
TranslationJob::TranslationJob(shared_ptr<TranslateRequest>req):
  id_(TranslationJobId()),req_(req),translated_(req->segs().size()),translations_(req->segs().size()){
}
// get id of job
TranslationJobId const&TranslationJob::id()const{
  return id_;
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
