#include <iostream>
#include <exception>
#include <fstream>
#include <list>
#include <set>
#include <iterator>
#include <vector>
#include <functional>
#include <algorithm>
#include <random>
#include <chrono>
#include <map>
#include <utility>
#include <unistd.h>
#include <type_traits>
#include <chrono>
#include <ratio>

using namespace std;

struct Foo{
  typedef string type;
  Foo(string const&s):s_(s){};
  string s_;
};

template<typename T>
void printit(typename T::type const& t){
  cout<<t<<endl;
}
template<typename T>
void printit(T t){
  cout<<t<<endl;
}

int main(){
  Foo foo("Foo");
  printit<Foo>(foo.s_);
  printit<int>(10);
}
