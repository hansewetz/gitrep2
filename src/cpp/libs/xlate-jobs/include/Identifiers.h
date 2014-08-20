#ifndef __IDENTIFIERS_H__
#define __IDENTIFIERS_H__
#include "utils/Id.h"
namespace xlate{

// request id
class TranslateRequest;
using TranslateRequestId=utils::Id<TranslateRequest,std::string,true>;

// job id
class TranslationJob;
using TranslationJobId=utils::Id<TranslationJob,std::string,true>;

// request id
class TranslationTask;
using TranslationTaskId=utils::Id<TranslationTask,std::string,true>;

// dummy engine id
class DummyEngine;
using DummyEngineId=utils::Id<DummyEngine,std::string,true>;
}
#endif
