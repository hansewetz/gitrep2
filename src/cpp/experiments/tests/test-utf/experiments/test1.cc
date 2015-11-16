// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015. All rights reserved.

#include "unicode.hpp"
#include<algorithm>
#include<vector>
using namespace std;

// print string in UTF8 hex
void printUtf8String(ostream&os,b2::utf8string const&str){
  auto const&cont=str.base_cont();
  for(auto it=cont.begin();it!=cont.end();++it){
    os<<hex<<"0x"<<static_cast<unsigned int>(*it)<<" ";
  }
  os<<endl;
}

// Main test program.
int main(){
  // Construct using codeunit iterators.
  b2::utf8string str1;
  str1.push_back(0xCAA4);
  str1.push_back(0x10852);
  printUtf8String(cout,str1);

  // pop last character
  str1.pop_front();
  printUtf8String(cout,str1);

// NOTE! Prints code points
  for(auto it=str1.begin();it!=str1.end();++it){
    cout<<*it<<" ";
  }
}
