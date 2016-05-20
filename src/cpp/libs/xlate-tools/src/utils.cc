// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015,2016. All rights reserved.

#include "xlate-tools/utils.h"
#include "xlate-jobs/TranslationTask.h"
#include <vector>
#include <algorithm>
using namespace std;
namespace xlate{

// get vector of tasks sorted by segment number form list of tasks
vector<shared_ptr<TranslationTask>>getTaskListSortedBySegNo(list<shared_ptr<TranslationTask>>tasklist){
  vector<shared_ptr<TranslationTask>>ret;
  copy(tasklist.begin(),tasklist.end(),back_inserter(ret));
  sort(ret.begin(),ret.end(),[](shared_ptr<TranslationTask>const&t1,shared_ptr<TranslationTask>const&t2){return t1->segno()<t2->segno();});
  return ret;
}
}
