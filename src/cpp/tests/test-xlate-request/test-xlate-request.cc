#include "xlate-jobs/TranslateRequest.h"
#include "xlate-jobs/TranslationJob.h"
#include <vector>
#include <iostream>
using namespace std;
using namespace xlate;

// main test program
int main(){
  TranslateRequest req{"Hello","world"};
  cout<<"request: "<<req<<endl;

  TranslationJob job("12345");
  cout<<"job: "<<job<<endl;
}
