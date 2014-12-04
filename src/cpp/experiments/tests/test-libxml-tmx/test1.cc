// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014. All rights reserved.

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <libxml/xmlreader.h>
using namespace std;


// states used to track where we should be
enum states{IDLE=0,HEADER=1,TU=2,TUV=3,SEG=4};
static states state=IDLE;

// We assume the file is correctly formatted so we can rely on our state
// (if we receive a tag we should not receive in the current state, then we toss an exception)
static void processNode(xmlTextReaderPtr reader){
  const xmlChar*name,*value;

  // tags to look for.
  const xmlChar*header_tag=(unsigned char*)"header";
  const xmlChar*prop_tag=(unsigned char*)"prop";
  const xmlChar*tuv_tag=(unsigned char*)"tuv";
  const xmlChar*tu_tag=(unsigned char*)"tu";
  const xmlChar*seg_tag=(unsigned char*)"seg";
  const xmlChar*text_tag=(unsigned char*)"#text";

  // get name of target together with value
  name = xmlTextReaderConstName(reader);
  value=xmlTextReaderConstValue(reader);

  // check what state we are in, and print source/target or target/source separator or new line
  switch(state){
    case IDLE:
      if(!xmlStrcmp(name,header_tag)){
        state=HEADER;
      }
      break;
    case HEADER:
      if(!xmlStrcmp(name,tu_tag)){
        state=TU;
      }
      break;
    case TU:
      if(!xmlStrcmp(name,tuv_tag)){
        state=TUV;
      if(!xmlStrcmp(name,text_tag))printf("%s",value);
     }
     break;
    case TUV:
      if(!xmlStrcmp(name,seg_tag)){
        state=SEG;
      }else
      if(!xmlStrcmp(name,tu_tag)){
        state=TU;
      }
      break;
    case SEG:
      if(!xmlStrcmp(name,tu_tag)){
        state=TU;
      }else
      if(!xmlStrcmp(name,tuv_tag)){
        state=TUV;
      }else
      if(!xmlStrcmp(name,text_tag)){
        printf("%s",value);
      }
      break;
  }
}
// process file
static void streamFile(const char*filename) {
  xmlTextReaderPtr reader;
  int ret;

  reader=xmlReaderForFile(filename,NULL,0);
  if (reader!=NULL) {
    ret=xmlTextReaderRead(reader);
    while (ret==1) {
      processNode(reader);
      ret=xmlTextReaderRead(reader);
    }
    xmlFreeTextReader(reader);
    if (ret!=0){
      fprintf(stderr,"%s : failed to parse\n",filename);
    }
  } else {
    fprintf(stderr,"Unable to open %s\n",filename);
  }
}
// main test program
int main(int argc, char **argv) {
  if (argc!=2){
    fprintf(stderr,"usage: %s <xml-file>\n",argv[0]);
    return(1);
  }
  streamFile(argv[1]);
  return(0);
}
