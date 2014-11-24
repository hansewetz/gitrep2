// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014. All rights reserved.

#include <iostream>
#include <memory>
using namespace std;

struct Foo{
  Foo(){cout<<"ctor"<<endl;}
  ~Foo(){cout<<"dtor"<<endl;}
};

/*
NOTE! Even if there are weak pointers, if the last shared pointer to the object is gone, the use count is 0.
	But, the weak pointer(s) will point to the use count containing a zero
	This means that we can maintain weak pointers even after the object is dead.
	--> Also, a weak pointer NEVER affects the reference count - it only observes it
*/

int main(){
  shared_ptr<Foo>sp;
  {
    sp=shared_ptr<Foo>(new Foo);
    cout<<"after creating sp: usecount (should be one): "<<sp.use_count()<<endl;
  }
  // shared pointerout of scope but object still alive
  weak_ptr<Foo>wp=sp;
  cout<<"after creating wweak pointer): usecount (should be one): "<<sp.use_count()<<endl;

  // create shared pointer
  {
    shared_ptr<Foo>sp2=wp.lock();
    cout<<"after locking weak pointer: usecount (should be two): "<<sp2.use_count()<<endl;
  }
  wp.reset();
  cout<<"after reset of weak pointer: usecount (should be zero): "<<wp.use_count()<<endl;
  cout<<"after reset of weak pointer: sp usecount (should be zero): "<<sp.use_count()<<endl;

  sp.reset();
  cout<<"after reset of shared: usecount (should be one): "<<sp.use_count()<<endl;

  // try to lock (create shared pointer)
  sp=wp.lock();
  cout<<"usecount (should be one): "<<sp.use_count()<<endl;
}
