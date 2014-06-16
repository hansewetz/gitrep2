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
#include <queue>
#include <chrono>
#include <ratio>
#include <cassert>

using namespace std;

// basic Node list
struct Node{
  Node(int v):val(v),next(0){}
  int val;
  Node*next;
};
Node*addnode(Node*head,Node*n){ // add node at beginning of list
  n->next=head;
  return n;
}
// detect if cycle in list
bool hascycle(Node*head){
  Node*n1step=head;
  Node*n2step=head;
  while(true){
    if(n1step==0)return false;

    n2step=n2step->next;
    if(n2step==0)return false;
    if(n2step==n1step)return true;

    n2step=n2step->next;
    if(n2step==0)return false;
    if(n2step==n1step)return true;

    n1step=n1step->next;
  }
}

int main(){
  Node*head=0;
  for(int i=0;i<10;++i)head=addnode(head,new Node(i));

  // get last node and loop around
  Node*n;
  for(n=head;n->next;n=n->next);
  n->next=head->next;

  // check if there is a loop
  cerr<<boolalpha<<"hascycle: "<<hascycle(head)<<endl;
}
