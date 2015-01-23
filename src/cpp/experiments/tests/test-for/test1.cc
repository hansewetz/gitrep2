// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015. All rights reserved.

#include <iostream>
#include <stdlib.h>
using namespace std;

// a collection representing random numbers
class Test{
public:
  // iterator should probably be written as a boost iterator facade
  class TestIter{
  public:
    // ctors
    TestIter(Test const*t):t_(t),count_(0),currVal_(t_->getRand()){}
    TestIter(Test const*t,int n):t_(t),count_(n){}
    TestIter(TestIter const&it)=default;
    TestIter&operator=(TestIter const&)=default;

    // compare, increment and deref
    bool operator!=(TestIter const&ti)const{return ti.count_!=count_&&ti.t_==t_;}
    TestIter&operator++(){++count_;currVal_=t_->getRand();return*this;}
    int operator*()const{return currVal_;}
  private:
    Test const*t_;
    int count_;
    int currVal_;
  };
  // ctors, assign, dtor
  Test(int maxCount):maxCount_(maxCount){}

  // begin, end iter
  TestIter begin()const{return TestIter{this};}
  TestIter end()const{return TestIter{this,maxCount_};}
private:
  int getRand()const{return rand();}
  int maxCount_;
};
// main test program
int main(){
  Test t(10);
  for(int r:t)cout<<r<<" ";
  cout<<endl;
}
