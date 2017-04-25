// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015,2016. All rights reserved.

#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <functional>
#include <memory>
using namespace std;

// P is a predicate returning true if P(first,other)==true
template<typename I,typename P>
I part(I start,I end,P p){
  typedef typename I::value_type value_type;

  I m=start;
  value_type t=*m;
  for(auto it=start+1;it!=end;++it){
    if(p(*it,t))iter_swap(it,++m);
  }
  iter_swap(m,start);
  return m;
}

int main(){
  vector<int>v={8,6,4,9,57,1,3,2,10,2,26};
  auto pred=std::less<int>();
  auto p=part(v.begin(),v.end(),pred);
  copy(v.begin(),v.end(),ostream_iterator<int>(cout," "));
  cerr<<endl<<"p: "<<*p<<endl;
  cerr<<"is partitioned: "<<boolalpha<<std::is_partitioned(v.begin(),v.end(),std::bind(pred,std::placeholders::_1,*p))<<endl;
}
