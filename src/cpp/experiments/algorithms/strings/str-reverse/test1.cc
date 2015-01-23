// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015. All rights reserved.

#include <iostream>
using namespace std;

string reverse(string&str){
  int start=0,end=str.length()-1;
  while(start<end)swap(str[start++],str[end--]);
  return str;
}
int main(){
  string s("123456789");
  cerr<<reverse(s)<<endl;
}
