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
  Buf*buf;
  Junk*j=new(buf)Junk;
}
