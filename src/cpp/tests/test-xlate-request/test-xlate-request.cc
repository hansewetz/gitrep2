#include "xlate-jobs/TranslateRequest.h"
#include "xlate-jobs/TranslationJob.h"
#include "xlate-jobs/TranslationTask.h"
#include "xlate-jobs/LanguageCode.h"
#include <vector>
#include <iostream>
#include <string>
using namespace std;
using namespace xlate;

// main test program
int main(){
  // create a request
  LanguageCode src{"en"};
  LanguageCode target{"sv"};
  TranslateRequest req{src,target,{"Hello","World"}};
  cout<<"REQ: "<<req<<endl;

  // create a job
  TranslationJob job(req);

  // do some operations on a a task within a job
  cout<<"JOB: "<<job<<endl;
  shared_ptr<TranslationTask>task{job.getNextTask()};
  cout<<"JOB: "<<job<<endl;
  job.addTranslatedTask(task);
  cout<<"JOB: "<<job<<endl;
}
