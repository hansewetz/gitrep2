// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015,2016. All rights reserved.

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
void printList(Node*head){
  while(head){
    cout<<head->data<<" ";
    head=head->next;
  }
}


// merge sort of single linked list
Node*merge(Node*left,Node*right){
  Node*head=0;
  Node*last=0;

  // make sure we have at leats one node in return list before zipping up the lists
  if(left&&right){
    if(left->data<right->data){
      head=left;
      last=left;
      left=left->next;
    }
    else{
      head=right;
      last=right;
      right=right->next;
    }
    last->next=0;
  }
  // add elements to return list as long as there are elements in both lists
  while(left&&right){
    if(left->data<right->data){
      last->next=left;
      last=last->next;
      left=left->next;
    }
    else{
      last->next=right;
      last=last->next;
      right=right->next;
    }
    last->next=0;
  }
  // add remaining left list (at most one list will be remaning so no needto move last ptr)
  if(left)last->next=left;

  // add remaining right list
  if(right)last->next=right;

  // done
  return head;
}
void split(Node*head,Node**left,Node**right){
  Node*slow=head;
  Node*fast=head;
  while(fast->next!=0){
    fast=fast->next;
    if(fast->next==0)break;
    fast=fast->next;
    slow=slow->next;
  }
  *left=head;
  *right=slow->next;
  slow->next=0;
}
Node*msort(Node*head){
  if(head==0||head->next==0)return head;

  // split list into two lists
  Node*left;
  Node*right;
  split(head,&left,&right);

  // sort each list
  left=msort(left);
  right=msort(right);

  // merge lists into one list
  return merge(left,right);
}
int main(){
  Node*head=0;
  head=addNode(head,new Node(4));
  head=addNode(head,new Node(2));
  head=addNode(head,new Node(8));
  head=addNode(head,new Node(1));
  head=addNode(head,new Node(15));
  head=addNode(head,new Node(-1));
  printList(head);
  cout<<endl;
  head=msort(head);
  printList(head);
  cout<<endl;
}
