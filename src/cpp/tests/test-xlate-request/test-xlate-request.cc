#include "xlate-jobs/TranslateRequest.h"
#include "xlate-jobs/TranslationJob.h"
#include "xlate-jobs/LanguageCode.h"
#include <vector>
#include <iostream>
using namespace std;
using namespace xlate;

// main test program
int main(){
  LanguageCode src{"en"};
  LanguageCode target{"sv"};
  TranslateRequest req(src,target,{"Hello","World"});
  cout<<req<<endl;

  TranslationJob job("12345");
  cout<<job<<endl;
}
