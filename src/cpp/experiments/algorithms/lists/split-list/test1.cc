// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015. All rights reserved.

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
#include <cstring>
#include <array>

using namespace std;

struct Node{
  Node(int d):data(d),next(0){}
  int data;
  Node*next;
};
Node*addNode(Node*head,Node*n){
  n->next=head;
  return n;
}
void printList(Node*head,ostream&os){
  while(head){
    os<<head->data<<" ";
    head=head->next;
  }
  os<<endl;
}
// split list into two lists
void partition(Node*head,Node**left,Node**right){
  if(head==0)return;
  Node*slow=head;
  Node*fast=head;
  while(fast->next){
    fast=fast->next;
    if(!fast->next)break;
    fast=fast->next;
    slow=slow->next;
  }
  *left=head;
  *right=slow->next;
  slow->next=0;
}

int main(){
  Node*head=0;
  head=addNode(head,new Node(3));
  head=addNode(head,new Node(2));
  head=addNode(head,new Node(-6));
  //head=addNode(head,new Node(17));
  //head=addNode(head,new Node(4));
  printList(head,cout);
  Node*left;
  Node*right;
  partition(head,&left,&right);
  printList(left,cout);
  printList(right,cout);
}
