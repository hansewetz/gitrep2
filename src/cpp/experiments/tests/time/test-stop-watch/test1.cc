// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015,2016. All rights reserved.

#include <chrono>
#include <ratio>

// old fashioned stopwatch
template<typename C,typename R=double>
class StopWatch{
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
  R getElapsedTimeHours()const{return getElapsedTime<std::chrono::duration<R,std::ratio<3600>>>();}
  R getElapsedTimeDays()const{return getElapsedTime<std::chrono::duration<R,std::ratio<3600*24>>>();}
  R getElapsedTimeWeeks()const{return getElapsedTime<std::chrono::duration<R,std::ratio<3600*24*7>>>();}

  // get state
  bool isRunning()const noexcept{return state_==State::running;}
  bool isStopped()const noexcept{return !isRunning();}
private:
  // typedefs and states
  using TP=typename C::time_point;
  using DURATION=typename C::duration;
  enum class Event:int{click=0,reset=1};
  enum class State:int{running=0,stopped=1};

  // state of stop watch
  C clock_;
  TP startTime_=TP();
  DURATION accumDuration_=DURATION::zero();
  State state_=State::stopped;

  // general method to read elapsed time expressed in type R units of P
  template<typename P>
  R getElapsedTime()const{
    DURATION currDuration{accumDuration_};
    if(state_==State::running)currDuration+=clock_.now()-startTime_;
    return std::chrono::duration_cast<P>(currDuration).count();
  }
  // state machine driving clock - returns current duration
  void fsm_(Event evnt){
    if(state_==State::running){
      if(evnt==Event::click){
        accumDuration_+=(clock_.now()-startTime_);
        state_=State::stopped;
      }else{
        accumDuration_=DURATION::zero();
        startTime_=clock_.now();
      }
    }else{
      if(evnt==Event::click){
        state_=State::running;
        startTime_=clock_.now();
      }else{
        accumDuration_=DURATION::zero();
        startTime_=TP();
      }
    }
  }
};  
// commonly used stop watches
using SystemStopWatch= StopWatch<std::chrono::system_clock,double>;
using HRStopWatch=StopWatch<std::chrono::high_resolution_clock,double>;
using SteadyStopWatch=StopWatch<std::chrono::steady_clock,double>;

#include <iostream>
#include <thread>
using namespace std;

// test program
int main(){
  SteadyStopWatch sw;

  string unit="micro";
  std::function<double()>elapsed=[&]()->double{return sw.getElapsedTimeMs();};

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
