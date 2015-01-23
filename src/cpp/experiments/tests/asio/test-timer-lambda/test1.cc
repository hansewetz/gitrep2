// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015. All rights reserved.

#include <iostream>
#include <functional>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
using namespace std;

// contger and max count
int cnt{0};
int maxCnt{5};

// io service
boost::asio::io_service ios;

// main.
int main(int argc,char**argv){
  // setup timer  (IO object)
  boost::asio::deadline_timer tmo(::ios,boost::posix_time::seconds(2));

  // completion handler for timer
  std::function<void(boost::system::error_code)>f=[&](boost::system::error_code err){
        // check iof we are onde
        cerr<<"TMO"<<endl;
        if(++cnt>maxCnt)return;

        // trigger timer again
        tmo.expires_from_now(boost::posix_time::seconds(2));
        tmo.async_wait(f);
      };

  // start timer (specify completion handler) and kick off event loop
  tmo.async_wait(f);
  ::ios.run();
}

