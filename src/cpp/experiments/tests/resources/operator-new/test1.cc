// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015. All rights reserved.

#include <iostream>
using namespace std;

struct Junk{
  Junk(){cout<<"Junk()"<<endl;}
  ~Junk(){cout<<"~Junk()"<<endl;}
};

template<typename T>
void*operator new(size_t s,T*t){
  cout<<"template operator new()"<<endl;
  return new char[s];
}
struct Buf{};

int main(){
  Buf buf;
  new(&buf)Junk;
}
