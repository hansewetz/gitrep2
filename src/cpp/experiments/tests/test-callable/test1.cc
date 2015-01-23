// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015. All rights reserved.

#include <type_traits>

// check if a class type is callable
// (Member Detector idiom)
template<typename T>
struct is_callable{
private:
  // create a derived type which may inherit from two types where one is sure to be callable
  struct fallback{void operator()();};
  struct derived:T,fallback{};

  // used for checking if derived contains a fallback method
  template<typename U,U>struct check;

  // if T does not contain a fallback method, then 'derived::operator() == fallback::operator()' and the first 'test' will be instantiated
  // if T does contain a fallback method, then 'derived::operator() != fallback::operator()' and the second 'test' will be instantiated
  //   since '&T::operator()' cannot be converted to 'void (is_callable<c1>::fallback::*)()'
  //   (SFINAE make sure the compiler won't bail out on the first declaration)
  //   (we must pass 'derived' as template parameter to ensure SFINAE)
  template<typename C>static std::false_type test(check<void(fallback::*)(),&C::operator()>*);
  template<typename>static std::true_type test(...);
public:
  constexpr static bool value=std::is_same<decltype(test<derived>(nullptr)),std::true_type>::value;
};
// NOTE!
//	add so that we can check even if type is a function


// test object
struct c1{
  void operator()(double,int,c1&);
};

#include <iostream>
#include <functional>
using namespace std;

// main.
int main(){
  // test fallback struct
  cout<<"c1 is fallback: "<<boolalpha<<is_callable<c1>::value<<endl;

  // test function
  function<int(double)>d2i=[](double d)->int{return static_cast<int>(d);};
  cout<<"lambda is "<<boolalpha<<is_callable<decltype(d2i)>::value<<endl;
}

