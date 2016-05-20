// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015,2016. All rights reserved.

#include "unicode/unistr.h"
#include <string>
#include <iostream>
using namespace std;

// main test program
int main(){
  // test string
  string utf8str("Hello World");

  // create unicode string, convert it to lower case and back to utf8
  string utf8lower;
  icu::UnicodeString::fromUTF8(utf8str).toLower().toUTF8String(utf8lower);

  // print result
  cout<<"lowercase: '"<<utf8lower<<"'"<<endl;
}
