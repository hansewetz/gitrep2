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

void reverseString(char*start,char*end){
  while(start<end)swap(*start++,*end--);
}
bool nextWord(char**start,char**end){
  while(**start==' '&&**start!='\0')++*start; // skip blanks
  if(**start=='\0')return false;
  *end=*start;
  while(**end!=' '&&**end!='\0')++*end;         // find next non-blank
  --*end;
  return *start!=*end;
}
void reverseWords(char*str){
  reverseString(str,str+strlen(str)-1);
  char*start=str;
  char*end;
  while(nextWord(&start,&end)){
    reverseString(start,end);
    start=end+1;
  }
}

int main(){
  char str[]="  Hello     world again";
  reverseWords(str);
  cerr<<"\""<<str<<"\""<<endl;
}
