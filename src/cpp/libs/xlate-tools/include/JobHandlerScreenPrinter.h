// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014. All rights reserved.

#ifndef __JOB_HANDLER_SCREEN_PRINTER_H__
#define __JOB_HANDLER_SCREEN_PRINTER_H__
#include "xlate-tools/JobHandler.h"
#include <iosfwd>
#include <memory>
namespace xlate{

// print  translate job to screen
class JobHandlerScreenPrinter:public JobHandler{
public:
  // ctors, assign, dtor
  JobHandlerScreenPrinter(std::shared_ptr<TranslationJob>job);
  JobHandlerScreenPrinter(JobHandlerScreenPrinter const&)=default;
  JobHandlerScreenPrinter(JobHandlerScreenPrinter&&)=default;
  JobHandlerScreenPrinter&operator=(JobHandlerScreenPrinter const&)=default;
  JobHandlerScreenPrinter&operator=(JobHandlerScreenPrinter&&)=default;
  virtual~JobHandlerScreenPrinter()=default;

  // virtual function call - print job to screen
  virtual void operator()();

  // print debug info about handler
  virtual void print(std::ostream&)const;
};
}
#endif
