// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015,2016. All rights reserved.

#include "indlist.h"
#include <tuple>
#include <string>
#include <sstream>
#include <functional>
#include <cxxabi.h>
using namespace std;
using namespace tutils;

// inner product of tuples
struct tuple_inner_product_helper{
  template<std::size_t...i,typename...TU>
  constexpr auto operator()(indlist<i...>const&,TU&&...tu)
      ->decltype(foldl_tuple(std::plus<int>(),maketuple()(apply_tuple(multiplies<int>(),transform_tuples<i>(std::forward<TU>(tu)...))...))){
    return foldl_tuple(std::plus<int>(),maketuple()(apply_tuple(multiplies<int>(),transform_tuples<i>(std::forward<TU>(tu)...))...));
  }
};
template<typename T1,typename...T2,
         typename TD=typename std::decay<T1>::type,
         typename IL=typename make_indlist_from_tuple<TD>::type>
constexpr auto tuple_inner_product(T1&&t1,T2&&...t2)->decltype(tuple_inner_product_helper()(IL(),std::forward<T1>(t1),std::forward<T2>(t2)...)){
  return tuple_inner_product_helper()(IL(),std::forward<T1>(t1),std::forward<T2>(t2)...);
}



// return argument
struct Type2String{
  template<typename T>
  string operator()(T&&){
    int status;
    return string("(")+abi::__cxa_demangle(typeid(T).name(),0,0,&status)+")";
  }
};
// concatenate arguments into a string
struct Concatenate2{
  template<typename T1,typename T2>
  string operator()(T1&&t1,T2&&t2){
    stringstream str;
    str<<t1<<t2;
    return str.str();
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

  // print the types of a tuple
  auto z1=std::make_tuple(1,"hello",2,std::plus<int>());
  cout<<transform_tuple(Type2String(),z1)<<endl;

  // pop front(s) on tuple
  auto a1=make_tuple(1,2,3,4);
  cout<<"popfront: "<<popn_front_tuple<2>(a1)<<endl;

  // fold tuple
  auto b=make_tuple("one=",1," two=",2," five=",5);
  cout<<"fold to strings: "<<foldl_tuple(Concatenate2(),b)<<endl;

  auto c=make_tuple(1,2,3,4,5,6);
  cout<<foldl_tuple(multiplies<int>(),c)<<endl;

  // create a tuple containing the type of each tuple elemnt
  auto d=make_tuple(1,"hello",string("world"),Type2String());
  cout<<transform_tuple(Type2String(),d)<<endl;

  // multiply elemnts in a tuple
  auto e=make_tuple(1,2,3,4,5);
  cout<<"5!="<<foldl_tuple(std::multiplies<int>(),e)<<endl;

  // check predicates
  using IL1=indlist<0,1,2>;
  auto f=make_tuple(1);
  cout<<IL()<<" is an indlist: "<<boolalpha<<is_indlist<IL>::value<<endl;
  cout<<f<<" is an indlist: "<<boolalpha<<is_indlist<decltype(e)>::value<<endl;
  cout<<IL()<<" is tuple: "<<boolalpha<<is_tuple<IL>::value<<endl;
  cout<<f<<" is a tuple: "<<boolalpha<<is_tuple<decltype(e)>::value<<endl;

  // do a inner product using foldl
  auto w1=make_tuple(1,2,3);
  auto w2=make_tuple(5,6,7);
  using IL3=make_indlist_from_tuple<decltype(w2)>::type;
  cout<<tuple_inner_product(w1,w2)<<endl;
}
