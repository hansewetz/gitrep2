#include <iostream>
using namespace std;

struct Junk{
  void junk()&{cout<<"l-value"<<endl;}
  void junk()&&{cout<<"r-value"<<endl;}
};

int main(){
  Junk j;
  j.junk();
  Junk().junk();
}
