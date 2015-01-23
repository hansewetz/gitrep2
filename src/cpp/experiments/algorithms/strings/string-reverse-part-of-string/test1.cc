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

void reverse(char*start,char*end){
  while(start<end)swap(*start++,*end--);
}

int main(){
  char str[]="Hello world again";
  reverse(&str[0],&str[0]+strlen(str)-1);
  cerr<<str<<endl;
}
