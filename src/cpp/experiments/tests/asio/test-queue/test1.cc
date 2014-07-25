#include "simple_queue.h"
#include "queue_listener.h"
#include <string>
#include <iostream>
using namespace std;

int main(){
  boost::asio::simple_queue<string>q1;
  q1.enq("Hello");
  q1.enq("World");
  while(!q1.empty()){
    pair<bool,string>res=q1.deq();
    cout<<res.second<<endl;
  }
}
