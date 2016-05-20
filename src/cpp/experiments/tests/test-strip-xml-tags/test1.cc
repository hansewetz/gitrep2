// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015,2016. All rights reserved.

#include <iostream>
#include <libxml/xmlreader.h>
using namespace std;

// define this if reading xml from file
#define READ_FROM_FILE

// check if xml string is empty (consists of only blanks, CR, LF)
bool xmlStringIsEmpty(xmlChar const*str){
  int len{xmlStrlen(str)};
  while(len!=0){
    xmlChar c=str[len-1];
    if(c==' '||c=='\t'||c=='\n'||c=='\r')--len;
    else break;
  }
  return len==0;
}
// process current node
// (print node if it is text)
static void processNode(xmlTextReaderPtr reader){
  // tags to look for.
  const xmlChar*text_tag=(unsigned char*)"#text";

  // get name of target together with value
  const xmlChar*name=xmlTextReaderConstName(reader);
  const xmlChar*value=xmlTextReaderConstValue(reader);

  // if we have a non-empty text node print it
  if(!xmlStrcmp(name,text_tag)&&value!=nullptr&&!xmlStringIsEmpty(value)){
    cout<<value;
  }
}
// process file
static void streamFile(const char*filename) {
  // create an xml reader
#ifdef READ_FROM_FILE
  xmlTextReaderPtr reader=xmlReaderForFile(filename,nullptr,0);
#else
  string const buf="<header> <hello/> <again></again> <source>This is the first source</source> <target>This is the first target</target> <source>This is the second source</source> <target>This is the second target</target> </header>";
  xmlTextReaderPtr reader=xmlReaderForMemory(buf.c_str(),buf.length(),"",NULL,0);
#endif
  // parse xml using reader
  if(reader!=nullptr){
    int ret;
    while ((ret=xmlTextReaderRead(reader))==1)processNode(reader);
    xmlFreeTextReader(reader);
    if (ret!=0)cerr<<"failed to parse: "<<filename<<endl;
  }else{
    cerr<<"Unable to open file: "<<filename<<endl;
  }
}
// main test program
int main(int argc, char **argv) {
  if (argc!=2){
    cerr<<"usage: "<<argv[0]<<" <xml-file>"<<endl;
    exit(1);
  }
  streamFile(argv[1]);
  exit(0);
}
