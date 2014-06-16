#include <iostream>
#include <exception>
#include <fstream>
#include <list>
#include <set>
#include <thread>
#include <mutex>
#include <iterator>
#include <vector>
#include <future>
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

class fdstreambuf:public std::streambuf{
public:
  fdstreambuf(int fd):fd_(fd){
    setp(buf_, buf_+1024); 
  }
  ~fdstreambuf(){
    close(fd_);
  }
protected:
  // buffer full
  int overflow(int c){
    char*ibegin=buf_;
    char*iend=pptr(); 
    setp(buf_,buf_+1024);
    int ilen=iend-ibegin;
    write(fd_,buf_,ilen);
    return c==-1?0:-1;
  }
  // sync with external media
  int sync(){
    return overflow(-1);
  }
private:
  int fd_;
  char buf_[1024];
};
class fdostream:public std::ostream{
public:
  fdostream(int fd):ostream(0),buf_(fd){
    rdbuf(&buf_);
  }
private:
  fdstreambuf buf_;
};

int main(){
  ifstream ifs("Makefile");
  fdostream fdos(1);
  while(true){
    string line;
    if(!getline(ifs,line))break;
    fdos<<line<<endl<<flush;
  }
}
