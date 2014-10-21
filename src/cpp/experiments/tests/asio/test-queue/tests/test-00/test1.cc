/*
This program tests that simple_queue::timed_deq() works
*/

#include <boost/asio_queue.h>
#include <boost/log/trivial.hpp>
#include <string>
#include <chrono>
using namespace std;
namespace asio=boost::asio;

// test program
int main(){
  try{
    // create queue
    std::chrono::milliseconds tmo{5000};
    size_t maxsize{1};
    asio::simple_queue<int>q{maxsize};

    // kick off a thread which enqueues a message
    std::function<void()>f=[&](){
      // sleep 2 seconds before deq()
      std::this_thread::sleep_for(std::chrono::milliseconds(6000));
      q.enq(17);
      BOOST_LOG_TRIVIAL(debug)<<"enqueued an item";
    };
    std::thread thr(f);

    // insert an element in queue
    BOOST_LOG_TRIVIAL(debug)<<"timed deq() ...";
    std::pair<bool,int>p=q.timed_deq(tmo);
    BOOST_LOG_TRIVIAL(debug)<<"deq(): "<<boolalpha<<p.first<<", "<<p.second;

    // join thread
    thr.join();
  }
  catch(exception const&e){
    BOOST_LOG_TRIVIAL(error)<<"cought exception: "<<e.what();
  }
}
