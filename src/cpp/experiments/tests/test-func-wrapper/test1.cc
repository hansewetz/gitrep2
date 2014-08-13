/*
implementation of pimpl idiom where constructor is a template and the derived implementation class is a template.
Usefull for packaged_tasks
*/
#include <iostream>
#include <memory>
#include <functional>
#include <future>
#include <string>

using namespace std;

// wraper for function
class fwrapper{
public:
  // base class for impl
  struct impl{
    virtual~impl(){}
    virtual void operator()()=0;
  };
  // derived class for impl
  template<typename F>
  struct fimpl:public impl{
    fimpl(F&&f):f_(std::move(f)){}
    void operator()(){
      f_();
    } 
    F f_;
  };
  // ctor
  template<typename F>
  fwrapper(F&&f):impl_(new fimpl<F>(std::move(f))){}

  // ctors, assign
  fwrapper(fwrapper const&fw)=delete;
  fwrapper(fwrapper&&fw)=default;
  fwrapper&operator=(fwrapper const&)=delete;
  fwrapper&operator=(fwrapper&&fw)=default;

  // call operator
  void operator()(){
    (*impl_)();
  }
  // pointer to implementation
  unique_ptr<impl>impl_;
};
// main.
int main(){
  std::packaged_task<string()>pt{[]()->string{return string("Hello");}};
  std::future<string>fut{std::move(pt.get_future())};
  fwrapper fw(std::move(pt));
  fw();
  cout<<"result: "<<fut.get()<<endl;
}
