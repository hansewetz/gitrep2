#include "xlate-jobs/TaskQueue.h"
#include <boost/asio.hpp> 
#include <boost/bind.hpp> 
#include <boost/log/trivial.hpp>
#include <iostream>
#include <thread>
#include <string>
using namespace std;
using namespace std::placeholders;
using namespace xlate;

// main test program
int main(){
  try{
  }
  catch(exception const&e){
    BOOST_LOG_TRIVIAL(debug)<<"cought exception: "<<e.what();
  }
}
