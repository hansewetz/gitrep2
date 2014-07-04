#include <chrono>
#include <ratio>
using namespace std;

// old fashioned stopwatch
template<typename C,typename R=double>
class StopWatch{
using TP=typename C::time_point;
using DURATION=typename C::duration;
enum class Event:int{click=0,reset=1,read=2};
enum class State:int{running=0,stopped=1};
public:
  // ctor, assign, dtor
  StopWatch()=default;
  StopWatch(StopWatch const&)=default;
  StopWatch(StopWatch&&)=default;
  StopWatch&operator=(StopWatch const&)=default;
  StopWatch&operator=(StopWatch&&)=default;
  ~StopWatch()=default;

  // start/top and reset
  void click(){fsm_(Event::click);}
  void reset(){fsm_(Event::reset);}

  // get elapsed time
  R getElapsedTimeNs()const{return getElapsedTime<std::chrono::duration<R,std::nano>>();}
  R getElapsedTimeUs()const{return getElapsedTime<std::chrono::duration<R,std::micro>>();}
  R getElapsedTimeMs()const{return getElapsedTime<std::chrono::duration<R,std::milli>>();}
  R getElapsedTimeSec()const{return getElapsedTime<std::chrono::duration<R,std::ratio<1>>>();}
  R getElapsedTimeMin()const{return getElapsedTime<std::chrono::duration<R,std::ratio<60>>>();}
  R getElapsedTimeHours()const{return getElapsedTime<std::chrono::duration<R,ratio<3600>>>();}
  R getElapsedTimeDays()const{return getElapsedTime<std::chrono::duration<R,ratio<3600*24>>>();}
  R getElapsedTimeWeeks()const{return getElapsedTime<std::chrono::duration<R,ratio<3600*24*7>>>();}

  // get state
  bool isRunning()const{return state_==State::running;}
  bool isStopped()const{return !isRunning();}
private:
  // state of stop watch
  C clock_;
  TP startTime_=TP();
  DURATION accumDuration_=DURATION();
  mutable DURATION currDuration_=DURATION();
  State state_=State::stopped;

  // general method to read elapsed time expressed in T type units of P (period as a duration)
  // (private since most likely no-one will call it directly)
  template<typename P>
  R getElapsedTime()const{
    const_cast<StopWatch<C,R>*>(this)->fsm_(Event::read);
    return std::chrono::duration_cast<P>(currDuration_).count();
  }
  // state machine driving clock
  // (push all updates of state here)
  void fsm_(Event evnt){
    switch(state_){
    case State::running:
      switch(evnt){
      case Event::click:
        accumDuration_+=chrono::duration_cast<DURATION>(clock_.now()-startTime_);
        state_=State::stopped;
        break;
      case Event::reset:
        startTime_=clock_.now();
        accumDuration_=DURATION();
        break;
      case Event::read:
        currDuration_=accumDuration_+chrono::duration_cast<DURATION>(clock_.now()-startTime_);
        break;
      }
      break;
    case State::stopped:
      switch(evnt){
      case Event::click:
        startTime_=clock_.now();
        state_=State::running;
        break;
      case Event::reset:
        startTime_=TP();
        accumDuration_=DURATION();
        break;
      case Event::read:
        currDuration_=accumDuration_;
        break;
      }
      break;
    }
  }
};  
// some typedefs to helpSystemStopWatch
using SystemStopWatch= StopWatch<std::chrono::system_clock,double>;
using HRStopWatch=StopWatch<std::chrono::high_resolution_clock,double>;

#include <iostream>
#include <thread>

// test program
int main(){
  HRStopWatch sw;

  string unit="micro";
  std::function<double()>elapsed=[&]()->double{return sw.getElapsedTimeUs();};

  // test starting clock ones then stopping it
  sw.click();
  std::this_thread::sleep_for(std::chrono::seconds(1));
  sw.reset();
  std::this_thread::sleep_for(std::chrono::seconds(1));
  sw.click();
  cerr<<"time in "<<unit<<": "<<elapsed()<<endl;       // read when clock is stopped

  // test starting again and make sure we accumulate time
  sw.click();
  this_thread::sleep_for(std::chrono::seconds(1));
  cerr<<"time in "<<unit<<": "<<elapsed()<<endl;       // read while clock is ticking
  sw.click();
  cerr<<"time in "<<unit<<": "<<elapsed()<<endl;       // read when clock is stopped
  sw.click();
  this_thread::sleep_for(std::chrono::seconds(1));
  cerr<<"time in "<<unit<<": "<<elapsed()<<endl;       // read while clock is ticking
  sw.click();
  this_thread::sleep_for(std::chrono::seconds(1));
  cerr<<"time in "<<unit<<": "<<elapsed()<<endl;       // read while clock is stopped
}
