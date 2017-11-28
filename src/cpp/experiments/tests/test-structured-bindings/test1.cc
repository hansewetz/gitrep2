#include <bits/stdc++.h>
using namespace std;

template<typename F,typename T,int...ind>
void apply(F&&f,T&&tu,integer_sequence<int,ind...>){
  using xxx=int[];
  xxx{(f(get<ind>(tu)),0)...};
}
// print a tuple
template<typename...Args>
ostream&operator<<(ostream&os,tuple<Args...>const&tu){
  apply([&os](auto const&t){os<<t<<" ";},tu,make_integer_sequence<int,sizeof...(Args)>());
  return os;
}
// test class to map structured bindings to
class Foo{
public:
  // tuple get
  template<std::size_t N>
  decltype(auto)get()const{
    if constexpr(N==0)return (tu_);
    else if constexpr(N==1)return (str_); // get reference
  }
  Foo(tuple<int,string>const&tu,string const&str):tu_(tu),str_(str){}
  decltype(auto)tu()const noexcept{return tu_;}
  string const&str()const noexcept{return str_;}
private:
  tuple<int,string>tu_;
  string str_;
};
// get tuple size
namespace std{
template<>
struct tuple_size<Foo>:std::integral_constant<size_t,2>{};

// get type of a tuple element
template<std::size_t N>
struct tuple_element<N,Foo>{
  using type=decltype(declval<Foo>().get<N>());
};
}


// test program
int main(){
  Foo foo{make_tuple(1,"World"s),"Hello"s};
  cout<<"Foo tuple size: "<<tuple_size<Foo>::value<<endl;
  auto const&[tu,str]=foo;
  cout<<"tu: "<<tu<<", str: "<<str<<endl;
}
