#include "xlate-tools/JobHandler.h"
#include <iostream>
using namespace std;
namespace xlate{

// ctor
JobHandler::JobHandler(std::shared_ptr<TranslationJob>job):job_(job){
}
// print operator
ostream&operator<<(ostream&os,JobHandler const&jh){
  jh.print(os);
  return os;
}
}
