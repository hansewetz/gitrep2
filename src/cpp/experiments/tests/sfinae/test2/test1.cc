// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015,2016. All rights reserved.

#include <iostream>
#include <type_traits>
using namespace std;

template<typename T,typename std::enable_if_t<!std::is_integral<T>::value>* =nullptr>
void foo(T t){
  cout<<"!is_integral: "<<t<<endl;
}
template<typename T,typename std::enable_if_t<std::is_integral<T>::value>* =nullptr>
void foo(T t){
  cout<<"is_integral: "<<t<<endl;
}
// test program
int main(){
  int d=6;
  foo(d);
}
