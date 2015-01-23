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

bool match(char*str,char*pattern){
  while(*pattern){
    if(*str++!=*pattern++)return false;
  }
  return true;
}
void replacePattern(char*str,char*pattern,char c){
  if(str==0||pattern==0)return;
  char*src=str;
  char*dest=str;
  char*end=str+strlen(str);
  while(src<end){
    if(match(src,pattern)){
      *dest++=c;
      src+=strlen(pattern);
    }
    else{
      *dest++=*src++;
    }
  }
  *dest='\0';
}

int main(){
  char str[]="Hello world again hello";
  char pattern[]="llo";
  replacePattern(str,pattern,'X');
  cerr<<str<<endl;
}
