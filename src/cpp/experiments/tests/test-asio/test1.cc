#include <thread>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
using namespace std;

std::condition_variable c;
std::mutex m;
int cnt = 0;

// timer cb
void tmoCb(const boost::system::error_code&err,boost::asio::deadline_timer*t){
  cerr<<"TMO"<<endl;
  std::unique_lock<mutex>l(m);
  ++cnt;
  l.unlock();
  c.notify_one();
  t->expires_from_now(boost::posix_time::seconds(2));
  t->async_wait(boost::bind(tmoCb,boost::asio::placeholders::error,t));
}  

// main.
int main(int argc,char**argv){
  boost::asio::io_service ios;
  boost::asio::deadline_timer tmo(ios);
  tmo.expires_from_now(boost::posix_time::seconds(2));
  tmo.async_wait(boost::bind(tmoCb,boost::asio::placeholders::error,&tmo));
  std::thread t(boost::bind(&boost::asio::io_service::run,&ios));

  // add a work thread so we never stop
  boost::asio::io_service::work work(ios);

  // wait until we have reached cnt==3, the stop iostervice
  std::unique_lock<mutex>l(m);
  c.wait(l,[&]()->bool{return cnt>=3;});
  ios.stop();
  t.join();
}

