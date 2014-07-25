#include "simpleq.h"
#include <string>
#include <iostream>
using namespace std;

int main(){
  simpleq<string>q1;
  q1.enq("Hello");
  q1.enq("World");
  while(!q1.empty()){
    pair<bool,string>res=q1.deq();
    cout<<res.second<<endl;
  }
}
