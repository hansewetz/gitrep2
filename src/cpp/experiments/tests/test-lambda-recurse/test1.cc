// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014. All rights reserved.

#include <iostream>
#include <vector>
#include <functional>
#include <algorithm>
using namespace std;

// main.
int main(int argc,char**argv){
  function<int(int)>fact=[&](int i){return i==0?1:i*fact(i-1);};
  cout<<"fact(5): "<<fact(5)<<endl;
}

