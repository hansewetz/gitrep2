#include <iostream>
#include <vector>
#include <variant>
#include <cxxabi.h>
using namespace std;

struct TypePrinter{
  template<typename T>
  void operator()(T const&t){
    int status;
    cout<<abi::__cxa_demangle(typeid(T).name(),0,0,&status)<<endl;
  }
};

// test program
int main(int argc,char*argv[]){
  using var_t=variant<int,string,vector<string>>;
  vector<var_t>v{"Hello"s,18,"Again"s,vector<string>{"one"s,"two"}};
  for(auto const&t:v){
    auto printtype=[](auto&&arg){TypePrinter{}(arg);};
    std::visit(printtype,t);
  }
}
