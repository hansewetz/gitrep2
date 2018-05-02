// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015,2016. All rights reserved.

#include <iostream>
#include <type_traits>
using namespace std;

template<typename T>
void foo(T t,typename std::enable_if<std::is_integral<T>::value>::type* =nullptr){
  cout<<"is_integral: "<<std::forward<T>(t)<<endl;
}
template<typename T>
void foo(T t,typename std::enable_if<!std::is_integral<T>::value>::type* =nullptr){
  cout<<"!is_integral: "<<t<<endl;
}

int main(){
  double d=6;
  foo(d);
}
