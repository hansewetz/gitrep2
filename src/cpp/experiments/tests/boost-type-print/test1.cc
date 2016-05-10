#include "boost/type_index.hpp"
#include <typeinfo>
#include <vector>
#include <iostream>
using namespace std;
using boost::typeindex::type_id_with_cvr;

int x;
auto f(){return (x);}
decltype(auto)g(){return (x);}

// test main
int main(){
  int const&j=5;
  auto i1=j;
  auto const&i2=j;

  int k;
  decltype(auto)i3=k;
  decltype(auto)i4=(k);
  decltype(auto)i5=std::move(k);

  cout<<"i1: "<<type_id_with_cvr<decltype(i1)>().pretty_name()<<endl;
  cout<<"i2: "<<type_id_with_cvr<decltype(i2)>().pretty_name()<<endl;
  cout<<"i3: "<<type_id_with_cvr<decltype(i3)>().pretty_name()<<endl;
  cout<<"i4: "<<type_id_with_cvr<decltype(i4)>().pretty_name()<<endl;
  cout<<"i5: "<<type_id_with_cvr<decltype(i5)>().pretty_name()<<endl;
  cout<<"f(): "<<type_id_with_cvr<decltype(f())>().pretty_name()<<endl;
  cout<<"g(): "<<type_id_with_cvr<decltype(g())>().pretty_name()<<endl;
}
