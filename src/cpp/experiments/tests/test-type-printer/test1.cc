// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014. All rights reserved.

#include <iostream>
#include <stdexcept>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/for_each.hpp>
#include <cxxabi.h>

using namespace std;
using namespace boost;

struct TypePrinter{
  template<typename T>
  void operator()(T const&t){
    int status;
    cout<<abi::__cxa_demangle(typeid(T).name(),0,0,&status)<<endl;
  }
};
// main.
int main(int argc,char**argv){
  typedef mpl::vector<int,long,double,string,TypePrinter>types;

  TypePrinter tp;
  mpl::for_each<types>(tp);
}
