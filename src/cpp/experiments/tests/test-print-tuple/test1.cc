#include <iostream>
#include <tuple>
using namespace std;

// print a tuple
template<typename T,typename F,size_t...i>
void printTuple(ostream&os,T&&tu,F&&f,integer_sequence<size_t,i...>){

  // make call within an initializer list --> order is ensured (evaluation is 1st el, 2nd el ...)
  using xxx=int[];
  (void)xxx{1,(std::forward<F>(f)(std::get<i>(std::forward<T>(tu))),0)...};
}
template<typename...Args>
ostream&operator<<(ostream&os,tuple<Args...>&&tu){
  printTuple(os,std::forward<decltype(tu)>(tu),[&os](auto x){os<<x<<" ";},make_integer_sequence<size_t,sizeof...(Args)>());
  return os;
}

// test program -------------------

// test program
int main(){
  auto tu=make_tuple(3,"Hello",3.7);
  cout<<"tu: "<<std::forward<decltype(tu)>(tu)<<endl;
}
