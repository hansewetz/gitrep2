// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015. All rights reserved.

#ifndef __JOB_HANDLER_H__
#define __JOB_HANDLER_H__
#include "xlate-jobs/TranslationJob.h"
#include <iosfwd>
#include <memory>
namespace xlate{

// handler for translated jobs
// (the handler can do anything it wants with the jobs since it is an abstract base class)
class JobHandler{
public:
  // print operator
  friend std::ostream&operator<<(std::ostream&,JobHandler const&);

  // ctors, assign, dtor
  JobHandler(std::shared_ptr<TranslationJob>job);
  JobHandler(JobHandler const&)=default;
  JobHandler(JobHandler&&)=default;
  JobHandler&operator=(JobHandler const&)=default;
  JobHandler&operator=(JobHandler&&)=default;
  virtual~JobHandler()=default;

  // virtual function call
  virtual void operator()()=0;

  // print debug info about handler
  virtual void print(std::ostream&)const=0;
protected:
  std::shared_ptr<TranslationJob>job_;
};
}
#endif
