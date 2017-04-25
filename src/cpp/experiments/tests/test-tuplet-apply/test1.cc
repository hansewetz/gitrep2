// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015,2016. All rights reserved.

#include <iostream>
#include <tuple>
#include <cxxabi.h>

// callable object
struct Print{
  template<typename T>
  void operator()(T const&t)const{
    std::cout<<"value: "<<t<<std::endl;
  }
};
struct PrintTypes{
  template<typename T>
  void operator()(T const&t)const{
    int status;
    std::cout<<abi::__cxa_demangle(typeid(T).name(),0,0,&status)<<std::endl;
  }
};
// meta function applying a function to each value in a tuple
template<std::size_t N,typename Func,typename Tuple>
struct Apply{
  static void execute(Func f,Tuple const&t){
    Apply<N-1,Func,Tuple>::execute(f,t);
    f(std::get<N-1>(t));
  }
};
template<typename Func,typename Tuple>
struct Apply<1,Func,Tuple>{
  static void execute(Func f,Tuple const&t){
    f(std::get<0>(t));
  }
};
// apply a function on each element in a tuple.
template<typename Func,typename ... Args>
void doapply(Func f,std::tuple<Args ...>const&t){
  Apply<sizeof ... (Args),Func,decltype(t)>::execute(f,t);
}
// main.
int main(){
  std::tuple<int,std::string,double>t{17,"hello",1.78};
  doapply(PrintTypes(),t);
  doapply(Print(),t);
}
