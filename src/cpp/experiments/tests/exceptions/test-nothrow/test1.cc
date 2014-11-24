// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014. All rights reserved.

#include <iostream>
#include <type_traits>
#include <stdexcept>
#include <exception>
#include <list>
using namespace std;

/*
	Shows that 'noexcept(...)' will call terminate if the function does throw when the condition inside nothrow is true
	Otherwise it won't throw.
*/

// throw execption
template<typename T>
void testfunc()noexcept(is_pod<T>::value){
  throw runtime_error("Testing");
}
// new terminate function
void my_terminate(){
  cerr<<"TERMINATING"<<endl;
  abort;
}
// main.
int main(int argc,char**argv){
  try{
    // cerr<<"testing with non-POD - should toss normal exception
    cerr<<"testing with non-POD - should toss exception"<<endl;
    testfunc<list<int>>();
  }
  catch(exception const&e){
    cerr<<"cought: "<<e.what()<<endl;
  }
  try{
    // set terminate function
    set_terminate(my_terminate);

    // cerr<<"testing with POD - should call 'terminate'
    cerr<<"testing with POD - should terminate"<<endl;
    testfunc<int>();
  }
  catch(exception const&e){
    // will never reach here - terminate aborts
    cerr<<"cought: "<<e.what()<<endl;
  }
}

