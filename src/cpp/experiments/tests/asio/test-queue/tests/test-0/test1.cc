/*
This program tests that simple_queue::timed_enq() works
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

    // insert an element in queue
    BOOST_LOG_TRIVIAL(debug)<<"enqing one item ...";
    bool stat1=q.timed_enq<decltype(tmo)>(17,tmo);
    BOOST_LOG_TRIVIAL(debug)<<"enq(): "<<boolalpha<<stat1;

    // kick off thread popping an element afetr 2 second
    std:;function<void(void)>ft=[&](){
        // sleep 2 seconds before deq()
        std::this_thread::sleep_for(std::chrono::milliseconds(6000));
        pair<bool,int>p=q.deq();
        BOOST_LOG_TRIVIAL(debug)<<"enq(): "<<boolalpha<<p.first<<", "<<p.second;
      };
    std::thread thr{ft};

    BOOST_LOG_TRIVIAL(debug)<<"enq wait seond item ...";
    bool stat2=q.timed_wait_enq<decltype(tmo)>(tmo);
    BOOST_LOG_TRIVIAL(debug)<<"enq_wait(): "<<boolalpha<<stat2;

    // join thread
    thr.join();
  }
  catch(exception const&e){
    BOOST_LOG_TRIVIAL(error)<<"cought exception: "<<e.what();
  }
}
