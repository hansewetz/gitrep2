// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015,2016. All rights reserved.

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
