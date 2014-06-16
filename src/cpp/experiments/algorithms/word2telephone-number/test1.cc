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

using namespace std;

int char2digit(char c){
  if(c=='0'||c=='1')return static_cast<int>(c-'0');
  return static_cast<int>(c-'A')/3+2;
}
string word2telno(string const&word){
  string telno;
  for(int i=0;i<word.length();++i){
    telno.push_back('0'+char2digit(word[i]));
  }
  return telno;
}
int main(){
  string word="CALLME";
  cerr<<"CALLME: "<<word2telno(word)<<endl;
}
