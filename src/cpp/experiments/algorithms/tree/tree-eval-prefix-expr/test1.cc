// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014. All rights reserved.

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
#include <stack>

using namespace std;

// evaluate prefix notation using one byte operands/operators
int evalPrefix(string const&expr,int&ind){
  if(expr.length()==0)return 0;
  char tok=expr[ind++];
  if(tok=='+'||tok=='*'||tok=='-'||tok=='/'){
    int val1=evalPrefix(expr,ind);
    int val2=evalPrefix(expr,ind);
    switch(tok){
      case '+': return val1+val2;
      case '-': return val1-val2;
      case '*': return val1*val2;
      case '/': return val1/val2;
    }
  }
  else{
    return tok-'0';
  }
}
// test program
int main(){
  string expr{"++*782/42"}; // (7*8+2)+(4/2)
  int const expected=(7*8+2)+(4/2);
  int ind=0;
  cout<<"result: "<<evalPrefix(expr,ind)<<endl;
  cout<<"expected: "<<expected<<endl;
}
