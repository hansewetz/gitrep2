#include "indlist.h"
#include <tuple>
#include <string>
#include <functional>
#include <cxxabi.h>
using namespace std;
using namespace tutils;

// return argument
struct Type2String{
  template<typename T>
  string operator()(T&&){
    int status;
    return string("(")+abi::__cxa_demangle(typeid(T).name(),0,0,&status)+")";
  }
};
// test function for tuple
struct PrintNValues{
  template<typename...T>
  void operator()(T&&...t){
    auto tu=make_tuple(std::forward<T>(t)...);
    cout<<tu<<endl;
  }
};
// print N values
struct Print1Value{
  template<typename T>
  void operator()(T&&t){cout<<t<<endl;}
};
// sum int values meta function
template<std::size_t i,std::size_t j>
struct sum{
  constexpr static std::size_t value=i+j;
};
template<std::size_t i,std::size_t j>
struct mul{
  constexpr static std::size_t value=i*j;
};
// main.
int main(int argc,char**argv){
  // transform a tuple using a meta function template (on indlist)
  auto tu=make_tuple(1,"two",3,"four",5);
  cout<<"xform pop_front template: "<<transform_tuple<pop_front>(tu)<<endl;

  // transform a tuple using an indlist (pick elemnts based on an indlist)
  auto tv=make_tuple(1,"two",3,"four",5);
  using IL=indlist<0,4>;
  cout<<"xform using indlist: "<<transform_tuple(IL(),tv)<<endl;

  // reverse a tuple
  auto tz=make_tuple(1,"two",3,"four",5);
  cout<<"reverse tuple1: "<<reverse_tuple(tz)<<endl;
  cout<<"reverse tuple2: "<<transform_tuple<reverse>(tz)<<endl;

  // transform tuples using an indlist by picking elemnst from the tuples
  auto t1=make_tuple(1,"one","ONE");
  auto t2=make_tuple(2,"two","TWO");
  auto t3=make_tuple(3,"three","THREE");
  cout<<"xform multiple tuples: "<<transform_tuples(indlist<0,1,2>(),t1,t2,t3)<<endl;

  // apply a function to ith elemnt of a set of tuples
  auto u1=make_tuple(1,"one","ONE");
  auto u2=make_tuple(2,"two","TWO");
  cout<<"sum of first elemnt of several tuples: "<<apply_tuple(std::plus<int>(),transform_tuples<0>(u1,u2))<<endl;

  // test fmaketuple<>
  Type2String tostr;
  auto z1=make_tuple(1,"hello",2);
  auto z2=ftransform_tuple(tostr,z1);
  cout<<"z2: "<<z2<<endl;
}
