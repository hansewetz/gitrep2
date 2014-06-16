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

char*remove(char*str,char const*rstr){
  // setup array so we can lookup if a charactre should be deleted ornot
  bool brem[128];
  fill(brem,brem+128,false);
  for(int i=0;i<strlen(rstr);++i)brem[static_cast<int>(rstr[i])]=true;

  if(strlen(rstr)==0)return str;
  char*dest=str;
  char*src=str;
  while(*src){
    if(brem[static_cast<int>(*src)]){
      ++src;
    }
    else{
      *dest++=*src++;
    }
  }
  *dest='\0';
  return str;
}
int main(){
  char str[]="Hello world";
  char rstr[]="lw";
  cerr<<"removed chars: "<<rstr<<" --> "<<remove(&str[0],&rstr[0])<<endl;
}
