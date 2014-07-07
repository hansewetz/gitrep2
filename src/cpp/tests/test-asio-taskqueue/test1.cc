#include "xlate-jobs/basic_taskq.h"
#include <boost/asio.hpp> 
#include <iostream>
using namespace std;
using namespace xlate;

// main test program
int main(){
  try{
    boost::asio::io_service ios; 
    TaskQueueIOService asioq{ios,make_shared<TaskQueue>(10),true};
  }
  catch(exception const&e){
    cerr<<"cought exception: "<<e.what()<<endl;
  }
}
