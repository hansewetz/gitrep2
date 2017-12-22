#include<bits/stdc++.h>
using namespace std;

class sigguard{
public:
  // ctor
  sigguard(sighandler_t fsig,auto...sigs):
      fsig_(fsig),oldhandlers_{setsig(sigs)...}{
  }
  // dtor
  ~sigguard(){
    for(auto it=rbegin(oldhandlers_);it!=rend(oldhandlers_);++it){
      signal(it->first,it->second);
    }
  }
  private:
    // set a single signal handler and return the old one so it can be set in the dtor
    decltype(auto)setsig(int sig){
      auto oldhandler{signal(sig,fsig_)};
      if(oldhandler==SIG_ERR)throw runtime_error(string("could not set signal handler: ")+strsignal(sig));
      return make_pair(sig,oldhandler);
    }
    // signal handler function
    sighandler_t fsig_;

    // old signal handlers
    initializer_list<pair<int,sighandler_t>>oldhandlers_;
};

struct Junk{
  operator int(){return sig_;}
  int sig_;
};

// test program
int main(int argc,char*argv[]){
  sigguard sg([](int sig){cout<<sig<<endl;},SIGHUP,SIGTERM,Junk{SIGCHLD});
}
