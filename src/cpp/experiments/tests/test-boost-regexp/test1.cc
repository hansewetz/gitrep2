// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014. All rights reserved.

#include <iostream>
#include <algorithm>
#include <sstream>
#include <boost/regex.hpp>
using namespace std;

// -------------- manual replacement

bool match(string::const_iterator sit,string::const_iterator send,string const&p){
  string::const_iterator pit{p.begin()};
  if(distance(sit,send)<p.length())return false;
  while(pit!=p.end()){
    if(*sit++!=*pit++)return false;
  }
  return true;
}
// genarel replacement of patterns with a string of equal or shorter length
string&replaceShortPattern(string&s,string const&pattern,string const&repl){
  size_t replLen{repl.length()};
  size_t patternLen{pattern.length()};
  string::iterator src=s.begin();
  string::iterator dest=s.begin();
  while(src!=s.end()){
    if(match(src,s.end(),pattern)){
      copy(repl.begin(),repl.end(),dest);
      src+=patternLen;
      dest+=replLen;
    }else{
     *dest++=*src++;
    }
  }
  s.erase(dest,s.end());
  return s;
}
string&removeSentenceMarkers(string&s){
  replaceShortPattern(s,string("<s>"),string(""));
  return replaceShortPattern(s,string("</s>"),string(""));
}
string&removeDuplicateSpaces(string&s){
  static constexpr char SPACE=' ';
  string::iterator end=s.end();
  string::iterator src=s.begin();
  string::iterator dest=s.begin();
  while(src!=end){
    if(*src==SPACE){
      while(++src!=end&&*src==SPACE);
      *dest++=SPACE;
    }else{
      *dest++=*src++;
    }
  }
  s.erase(dest,end);
  return s;
}
string&replacePipeWithSpace(string&s){
  return replaceShortPattern(s,string("|"),string(" "));
}
string&removeBackslashBeforeSemicolon(string&s){
  return replaceShortPattern(s,string("\\;"),string(";"));
}
// -------------- regular expression replacement
#if FALSE
// filter out <s>, </s>
string removeSentenceMarkers(string const&s){
  static const boost::regex PATTERN("<s>|</s>");
  ostringstream str;
  str<<boost::regex_replace(s,PATTERN,"");
  return str.str();
}
// replace multiple spaces with a single space
string removeDuplicateSpaces(string const&s){
  static const boost::regex PATTERN("\040+");
  ostringstream str;
  str<<boost::regex_replace(s,PATTERN," ");
  return str.str();
}
// replace PIPE with SPACE
string replacePipeWithSpace(string const&s){
  static const boost::regex PATTERN("\\|");
  ostringstream str;
  str<<boost::regex_replace(s,PATTERN," ");
  return str.str();
}
// remove BACKSLASH before SEMICOLON
string removeBackslashBeforeSemicolon(string const&s){
  static const boost::regex PATTERN("\\\\;");
  ostringstream str;
  str<<boost::regex_replace(s,PATTERN,";");
  return str.str();
}
#endif

// test program
int main(){
  // test removal of sentence markers
  string line1{"<s>First Content</s>. <s>Hello</s>"};
  string line11{line1};
  removeSentenceMarkers(line1);
  cout<<"orig string: \""<<line11<<"\", filtered string: \""<<line1<<"\""<<endl;

  // test removal of duplicate spaces
  string line2{"Hello  again   world.     "};
  string line22{line2};
  removeDuplicateSpaces(line2);
  cout<<"orig string: \""<<line22<<"\", filtered string: \""<<line2<<"\""<<endl;

  // test replacing pipe with space
  string line3{"Hello|again|world."};
  string line33{line3};
  replacePipeWithSpace(line3);
  cout<<"orig string: \""<<line33<<"\", filtered string: \""<<line3<<"\""<<endl;

  // test removal backslash before semi colon
  string line4{"Hello \\; world\\; again"};
  string line44{line4};
  removeBackslashBeforeSemicolon(line4);
  cout<<"orig string: \""<<line44<<"\", filtered string: \""<<line4<<"\""<<endl;
}
