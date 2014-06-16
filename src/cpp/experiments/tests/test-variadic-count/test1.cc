#include <iostream>
#include <type_traits>
using namespace std;

// count number of parameters in parameter pack
constexpr int count(){
  return 0;
}
template<typename T,typename ... Args>
constexpr int count(T,Args ... args){
  return 1+count(args ...);
}
// main.
int main(int argc,char**argv){
  cout<<count(1,"hello",18)<<endl;
}
