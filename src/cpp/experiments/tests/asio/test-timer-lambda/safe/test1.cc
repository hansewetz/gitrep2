#include <iostream>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
using namespace std;

// contger and max count
int cnt{0};
int maxCnt{5};

// main.
int main(int argc,char**argv){
  // io service
  boost::asio::io_service ios;

  // setup timer 
  boost::asio::deadline_timer tmo(ios,boost::posix_time::seconds(2));
  tmo.async_wait([&](boost::system::error_code err){
        cerr<<"TMO"<<endl;
      });

  // run event koop
  ios.run();
}

