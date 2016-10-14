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

// test program
int main(){
  char str[]={"012342789012"};
  cout<<str<<endl;
  remdupl(str);
  cout<<str<<endl;
}
