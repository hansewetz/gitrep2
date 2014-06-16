#include <iostream>
using namespace std;

// main test program
int main(){
  auto pf=[](auto t){cout<<t<<endl;}
  pf(15);
  pf("fifteen");
}
