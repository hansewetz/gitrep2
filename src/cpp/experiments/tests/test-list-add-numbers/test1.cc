// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015. All rights reserved.

#include <iostream>
#include <list>
#include <iterator>
using namespace std;
/*
You have two numbers represented by a linked list, where each node contains a single
digit. The digits are stored in reverse order, such that the 1’s digit is at the head of
the list. Write a function that adds the two numbers and returns the sum as a linked
list.
*/

list<int>add(list<int>const&l1,list<int>const&l2){
  list<int>ret;
  int carry=0;
  list<int>::const_iterator it1=l1.begin();
  list<int>::const_iterator it2=l2.begin();
  for(;it1!=l1.end(),it2!=l2.end();++it1,++it2){
    int num=*it1+*it2+carry;
    int val=num%10;
    carry=num/10;
    ret.push_back(val);
  }
  list<int>::const_iterator it=(it2==l2.end())?it1:it2;
  list<int>::const_iterator end=(it2==l2.end())?l1.end():l2.end();
  for(;it!=end;++it){
    int num=*it+carry;
    int val=num%10;
    carry=num/10;
    ret.push_back(val);
  }
  if(carry!=0)ret.push_back(carry);
  return ret;
}
int main(){
  list<int>l1={8,2,3,4,7,1};
  list<int>l2={2,3,4,5};
  list<int>res{add(l1,l2)};
  copy(res.begin(),res.end(),ostream_iterator<int>(cout," "));
}

