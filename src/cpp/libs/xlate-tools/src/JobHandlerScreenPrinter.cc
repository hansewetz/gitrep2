#include "xlate-tools/JobHandlerScreenPrinter.h"
#include "xlate-jobs/TranslationTask.h"
#include "xlate-jobs/TranslationJob.h"
#include <iostream>
#include <memory>
using namespace std;
namespace xlate{

// ctor
JobHandlerScreenPrinter::JobHandlerScreenPrinter(shared_ptr<TranslationJob>job):JobHandler(job){
}
// virtual function call - print job to screen
void JobHandlerScreenPrinter::operator()(){
  // print translated segments
  cout<<endl<<"------------------------------------- job#: "<<job_->id()<<endl;
  list<shared_ptr<TranslationTask>>const&translated{job_->translated()};
  for(shared_ptr<TranslationTask>task:translated)cout<<"["<<task->srcSeg()<<"]-->["<<task->targetSeg()<<"][segno: "<<task->segno()<<"][id: "<<task->id()<<"][engine: "<<task->engineId()<<"]"<<endl;
  cout<<"------------------------------------------------------------------------------"<<endl;
}
// print debug info about handler
void JobHandlerScreenPrinter::print(ostream&os)const{
  os<<"JobHandlerScreenPrinter";
}
}
