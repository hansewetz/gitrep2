#include "xlate-jobs/TranslateRequest.h"
#include "xlate-jobs/TranslationJob.h"
#include "xlate-jobs/LanguageCode.h"
#include <vector>
#include <iostream>
#include <memory>
using namespace std;
using namespace xlate;

// main test program
int main(){
  // create a request
  LanguageCode src{"en"};
  LanguageCode target{"sv"};
  shared_ptr<TranslateRequest>req{new TranslateRequest(src,target,{"Hello","World"})};
  cout<<*req<<endl;

  // create a job
  TranslationJob job(req);
  cout<<job<<endl;
}
