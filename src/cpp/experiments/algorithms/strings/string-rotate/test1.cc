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

using namespace std;

void reverse(char*start,char*end){
  while(start<end)swap(*start++,*end--);
}
void rotate(char*str,std::size_t k){
  if(str==0||k==0||k==strlen(str))return;
  std::size_t n=strlen(str);
  k%=n;
  reverse(str,str+k-1);
  reverse(str+k,str+n-1);
  reverse(str,str+n-1);
}

int main(){
  char str[]="0123456789";
  rotate(str,3);
  cerr<<str<<endl;
}
