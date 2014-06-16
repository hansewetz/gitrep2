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

// convert a tel digit to a character (index by 0,1,2)
char digit2char(int digit,int ind){
  return 'A'+(digit-2)*3+ind;
}
void printTelNoPermsAux(string const&telNo,string&res,int startInd){
  // if start index of remainder of telNo == length of telNo then we are done
  if(startInd==telNo.length()){
    cout<<res<<endl;
    return;
  }
  char digit=telNo[startInd];

  // 0 and 1 do not have any characters
  if(digit=='0'||digit=='1'){
    res.push_back(telNo[startInd]);
    printTelNoPermsAux(telNo,res,startInd+1);
    res.pop_back();
    return;
  }
  // for each character in current digit (3 of them), generate remainding character combinations
  for(int j=0;j<3;++j){
    res.push_back(digit2char(digit-'0',j));
    printTelNoPermsAux(telNo,res,startInd+1);
    res.pop_back();
  }
}
void printTelNoPerms(string const&telNo){
  string tmp;
  printTelNoPermsAux(telNo,tmp,0);
}

int main(){
  string telNo{"225563"};
  printTelNoPerms(telNo);
}
