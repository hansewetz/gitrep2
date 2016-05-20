// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015,2016. All rights reserved.

#include "boost/type_index.hpp"
#include <typeinfo>
#include <vector>
#include <iostream>
using namespace std;
using boost::typeindex::type_id_with_cvr;

// test main
int main(){
  vector<int>v{1,2,3,4};
  cout<<"v.size(): "<<v.size()<<endl;
  auto g=[w=std::move(v)](int x){return w[x];};
  cout<<"v.size(): "<<v.size()<<endl;
  cout<<"g(3): "<<g(3)<<endl;
}
