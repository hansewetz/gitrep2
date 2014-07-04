#include <iostream>
#include <fstream>
#include <list>
#include <memory>
using namespace std;

int main(){
  ifstream ifs("Makefile");
  std::function<void(ifstream*)>closer([](ifstream*ifs){cerr<<"closing ...";ifs->close();cerr<<" closed"<<endl;});
  unique_ptr<ifstream,std::function<void(ifstream*)>>uifs(&ifs,closer);
}

