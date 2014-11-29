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
