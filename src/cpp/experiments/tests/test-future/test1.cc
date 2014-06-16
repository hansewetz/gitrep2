#include <iostream>
#include <string>
#include <thread>
#include <future>
#include <tuple>
using namespace std;

// genera case
template<size_t N,typename TUP>
struct PrintTuple{
  static void print(TUP const&t){
    PrintTuple<N-1,TUP>::print(t);
    cout<<get<N-1>(t)<<" ";
  }
};
// end case
template<typename TUP>
struct PrintTuple<0,TUP>{
  static void print(TUP const&t){};
};

// wrapper
template<typename ...ARGS>
void print(tuple<ARGS...>const&t){
  PrintTuple<sizeof...(ARGS),decltype(t)>::print(t);
}
// main.
int main(){
  tuple<int,string,double>tup{17,string("Hello"),1.78};
  print(tup);
}
