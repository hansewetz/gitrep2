// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015. All rights reserved.

#include <stdio.h>
#include <string.h>
#include <libxml/xmlreader.h>

// state specifying if we are in source, in target or outside either of them
enum states{outside,insource,intarget};
static states state=outside;

// We assume the file is correctly formatted so we can run a state machine and not get any invalid state transitions.
// (that is, we assume the following: <source> ... </source> ... <target> ... </target>)
static void processNode(xmlTextReaderPtr reader){
  const xmlChar*name,*value;

  // tags to look for.
  // NOTE! We should probably take the 'seg-source' and 'target' tags. But each one of these are segmented so
  //     we would have to save the segments in a source-list/target-list and match them up ones we leave the target.
  //     When leaving target kill list and create new lists when entering a source.
  //     Right now we take the source (unsegmented) and the target (segmented) and match them.
  const xmlChar*source_tag=(unsigned char*)"source";
  const xmlChar*target_tag=(unsigned char*)"target";
  const xmlChar*text_tag=(unsigned char*)"#text";

  // get name of target together with value
  name = xmlTextReaderConstName(reader);
  value=xmlTextReaderConstValue(reader);

  // check what state we are in, and print source/target or target/source separator or new line
  switch(state){
    case outside:
      if(!xmlStrcmp(name,source_tag)){
       state=insource;
       if(!xmlStrcmp(name,text_tag))printf("%s",value);
      }else
      if(!xmlStrcmp(name,target_tag)){
       state=intarget;
       if(!xmlStrcmp(name,text_tag))printf("%s",value);
      }
      break;
    case insource:
      if(!xmlStrcmp(name,source_tag)){
        state=outside;
        printf(" --> ");    // NOTE! Here we separate source from target
      }else
      if(!xmlStrcmp(name,text_tag))printf("%s",value);
      break;
    case intarget:
      if(!xmlStrcmp(name,target_tag)){
        state=outside;
        printf("\n\n");     // NOTE! separate next source/target pair
      }else
     if(!xmlStrcmp(name,text_tag))printf("%s",value);
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
