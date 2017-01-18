#include <stdlib.h>
#include <iostream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <map>
using namespace std;

// remove duplcate characters in string
void remdupl(char*str){
  // src always increasing, dest increases when non-dupl char
  char*src=str;
  char*dest=str;
  while(*src){
    // check if there is a character to the left of dest == *src
    bool found=false;
    char c=*src;
    for(char*p=str;p<dest;++p){
      if(c==*p){
        found=true;
        break;
      }
    }
    if(found){
      ++src;                    // skip duplicate character
    }else{
      *dest++=*src++;           // keep src character
    }
  }
  *dest='\0';
}
// replace ' ' with '%20'
void repl(string&str){
  int len=str.size();

  // count spaces
  int nspaces=0;
  for(int i=0;i<static_cast<int>(str.size());++i){
    if(str[i]==' ')++nspaces;
  }
  // resize string
  int newlen=len+nspaces*2;
  str.resize(newlen,'X');

  // loop from back keep src + dest indices
  int dest=newlen-1;
  for(int src=len-1;src>=0;--src){
    if(str[src]==' '){
      str[dest--]='0';
      str[dest--]='2';
      str[dest--]='%';
    }else{
      str[dest--]=str[src];
    }
  }
}

// test program
int main(){
  char str[]={"012342789012"};
  cout<<str<<endl;
  remdupl(str);
  cout<<str<<endl;

  string str1{"Hello World   Again  "};
  repl(str1);
  cout<<str1<<endl;
}
