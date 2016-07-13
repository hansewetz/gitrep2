// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015,2016. All rights reserved.

#include <iostream>
using namespace std;

template<typename ...Args>
auto mysum(Args&&...args){
  return (std::forward<Args>(args)+...);
}

int main(){
  auto s=mysum(1,2,3,4);
  cout<<s<<endl;
}
