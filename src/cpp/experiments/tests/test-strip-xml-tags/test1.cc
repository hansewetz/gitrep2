// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015. All rights reserved.

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
// We assume the file is correctly formatted so we can run a state machine and not get any invalid state transitions.
// (that is, we assume the following: <source> ... </source> ... <target> ... </target>)
static void processNode(xmlTextReaderPtr reader){
  const xmlChar*name,*value;

  // tags to look for.
  const xmlChar*text_tag=(unsigned char*)"#text";

  // get name of target together with value
  name=xmlTextReaderConstName(reader);
  value=xmlTextReaderConstValue(reader);

  // check what state we are in, and print source/target or target/source separator or new line
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
