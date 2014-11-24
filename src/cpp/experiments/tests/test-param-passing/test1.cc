// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014. All rights reserved.

#include <iostream>
//#include <exception>
//#include <fstream>
//#include <list>
//#include <set>
//#include <thread>
//#include <mutex>
//#include <iterator>
//#include <vector>
//#include <future>
//#include <functional>
//#include <algorithm>
//#include <random>
//#include <chrono>
//#include <map>
//#include <utility>
//#include <unistd.h>
#include <type_traits>
#include <chrono>
#include <ratio>

using namespace std;

// type converter for passing parameters
template<typename T>
struct param{
  typedef typename std::conditional<std::is_integral<T>::value,T,typename std::add_lvalue_reference<const T>::type>::type type;
};

// dummy
void bar();

// a sample function for testing type converter
template<typename T>
__attribute__ ((noinline)) void foo_optimized(typename param<T>::type t){
  //cerr<<boolalpha<<"parameter type is reference: "<<is_reference<decltype(t)>::value<<endl;
  bar();
}
template<typename T>
__attribute__ ((noinline)) void foo_normal(T const&t){
  //cerr<<boolalpha<<"parameter type is reference: "<<is_reference<decltype(t)>::value<<endl;
  bar();
}
int main(){
  using Clock=chrono::high_resolution_clock;
  using TP=Clock::time_point;
  using DURATION=Clock::duration;
  using PARAM=unsigned long long;

  Clock clock;
  const size_t niter=1000000000;

  TP tpStart1{clock.now()};
  for(PARAM i=0;i<niter;++i)foo_normal<PARAM>(i);
  auto duration1=chrono::duration_cast<DURATION>(clock.now()-tpStart1);
  auto t1=chrono::duration_cast<chrono::duration<double,std::milli>>(duration1).count();

  TP tpStart2{clock.now()};
  for(PARAM i=0;i<niter;++i)foo_optimized<PARAM>(i);
  auto duration2=chrono::duration_cast<DURATION>(clock.now()-tpStart2);
  auto t2=chrono::duration_cast<chrono::duration<double,std::milli>>(duration2).count();

  cerr<<"milli sec time passing integer by reference: "<<t1<<endl;
  cerr<<"milli sec time passing integer by value: "<<t2<<endl;
  cerr<<"%time decrease: "<<100*(t1-t2)/t1<<"%"<<endl;
}
