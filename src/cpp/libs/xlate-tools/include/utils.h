// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015. All rights reserved.

#ifndef __UTILS_H__
#define __UTILS_H__
#include <vector>
#include <list>
#include <memory>
namespace xlate{

// forward decl
class TranslationTask;

// get vector of tasks sorted by segment number form list of tasks
std::vector<std::shared_ptr<TranslationTask>>getTaskListSortedBySegNo(std::list<std::shared_ptr<TranslationTask>>tasklist);

}
#endif
