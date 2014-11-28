// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014. All rights reserved.

#include "xlate-tools/JobHandlerScreenPrinter.h"
#include "xlate-tools/utils.h"
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
  // sort tasks by segno
  vector<shared_ptr<TranslationTask>>tasks{getTaskListSortedBySegNo({job_->translated()})};

  // print translated segments
  cout<<endl<<"------------------------------------- job#: "<<job_->id()<<" -------------------------------------"<<endl;
  for(shared_ptr<TranslationTask>task:tasks)cout<<"["<<task->srcSeg()<<"]-->["<<task->targetSeg()<<"][segno: "<<task->segno()<<"][id: "<<task->id()<<"][engine: "<<task->engineId()<<"]"<<endl;
  cout<<"------------------------------------------------------------------------------"<<endl;
}
// print debug info about handler
void JobHandlerScreenPrinter::print(ostream&os)const{
  os<<"JobHandlerScreenPrinter";
}
}
