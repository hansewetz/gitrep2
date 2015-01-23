// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015. All rights reserved.

#include <iostream>
#include <string>
using namespace std;

class Test{
public:
  Test()=default;
  Test(Test const&)=default;
  Test&operator()(Test const&)=delete;
  string const&getX()const{return x_;}
private:
  const std::string x_="Hello";
};

int main(){
  Test t;
  cerr<<"t.x: "<<t.getX()<<endl;
  Test t2{t};
}
