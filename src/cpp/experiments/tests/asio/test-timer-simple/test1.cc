#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
using namespace std;

// contger and max count
int cnt{0};
int mac_count{4};

// timer cb
void tmoCb(const boost::system::error_code&err,boost::asio::deadline_timer*t){
  cerr<<"TMO"<<endl;
  ++cnt;
  if(cnt>=mac_count)return;
  t->expires_from_now(boost::posix_time::seconds(2));
  t->async_wait(boost::bind(tmoCb,boost::asio::placeholders::error,t));
}  

// main.
int main(int argc,char**argv){
  // io service
  boost::asio::io_service ios;

  // setup timer 
  boost::asio::deadline_timer tmo(ios,boost::posix_time::seconds(2));
  tmo.async_wait(boost::bind(tmoCb,boost::asio::placeholders::error,&tmo));

  // run event koop
  ios.run();
}

