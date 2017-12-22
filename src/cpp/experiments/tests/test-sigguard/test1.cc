#include<stack>
#include <iostream>
#include <utility>
#include <csignal>
#include <cstring>

template<int...SIGS>
class sigguard{
public:
  // ctor
  sigguard(sighandler_t fsig):fsig_(fsig){
    setsigs(std::integer_sequence<int,SIGS...>());
  }
  // dtor
  ~sigguard(){
    while(!oldhandlers_.empty()){
      auto const[sig,fsig]=oldhandlers_.top();
      oldhandlers_.pop();
      signal(sig,fsig);
    }
  }
  private:
    // setup all new signal handlers
    template<int...sigs>
    void setsigs(std::integer_sequence<int,sigs...>){
      using xxx=int[];
      (void)xxx{(setsig(sigs),0)...};
    }
    // set a single signal handler and save the old one so it can be set in the dtor
    void setsig(int sig){
      auto oldhandler=signal(sig,fsig_);
      if(oldhandler==SIG_ERR){
        throw std::runtime_error(std::string("could not set signal handler: ")+strsignal(sig));
      }
      oldhandlers_.push(std::make_pair(sig,oldhandler));
    }
    // old signal handlers
    std::stack<std::pair<int,sighandler_t>>oldhandlers_;

    // signal handler function
    sighandler_t fsig_;
};
// test program
int main(){
  {
    // install a signal handler for signals
    //sigguard<SIGINT,SIGHUP>([](int sig){});
    auto sg=sigguard<SIGHUP,SIGINT>([](int sig){});

    // code, code, code ...
    // ...
  }
}
