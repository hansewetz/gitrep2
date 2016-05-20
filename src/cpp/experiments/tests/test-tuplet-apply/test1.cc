// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015,2016. All rights reserved.

#include <iostream>
#include <tuple>
#include <cxxabi.h>
using namespace std;

// callable object
struct Print{
  template<typename T>
  void operator()(T const&t)const{
    cout<<"value: "<<t<<endl;
  }
};
struct PrintTypes{
  template<typename T>
  void operator()(T const&t)const{
    int status;
    cout<<abi::__cxa_demangle(typeid(T).name(),0,0,&status)<<endl;
  }
};
// meta function applying a function to each value in a tuple
template<int N,typename Func,typename Tuple>
struct Apply{
  static void execute(Func f,Tuple const&t){
    Apply<N-1,Func,Tuple>::execute(f,t);
    f(get<N-1>(t));
  }
};
template<typename Func,typename Tuple>
struct Apply<1,Func,Tuple>{
  static void execute(Func f,Tuple const&t){
    f(get<0>(t));
  }
};
// apply a function on each element in a tuple.
template<typename Func,typename ... Args>
void apply(Func f,tuple<Args ...>const&t){
  Apply<sizeof ... (Args),Func,decltype(t)>::execute(f,t);
}
// main.
int main(){
  tuple<int,string,double>t{17,"hello",1.78};
  apply(PrintTypes(),t);
  apply(Print(),t);
}
