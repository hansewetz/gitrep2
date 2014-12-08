// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014. All rights reserved.

#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <stdio.h>
#include <string.h>
#include <libxml/xmlreader.h>
#include <boost/algorithm/string.hpp>
using namespace std;

// tags to look for.
const xmlChar*header_tag=(unsigned char*)"header";
const xmlChar*prop_tag=(unsigned char*)"prop";
const xmlChar*tuv_tag=(unsigned char*)"tuv";
const xmlChar*tu_tag=(unsigned char*)"tu";
const xmlChar*seg_tag=(unsigned char*)"seg";
const xmlChar*text_tag=(unsigned char*)"#text";

// various property text strings
const xmlChar*dbid_attr_value=(unsigned char*)"x-TMName";

// valid languages
static vector<const xmlChar*>lans{(unsigned char*)"nb-NO",(unsigned char*)"nn-NO",(unsigned char*)"en-GB"};

// state related variables
enum states{NOT_STARTED=0,HEADER=1,TU=2,TUV=3,SEG=4};
static states state=NOT_STARTED;
static bool getdbidFromHeader{false};
static string currLan;
static string currseg;

// check if tag contains a specific attribute
bool tagHasDbid(xmlNodePtr node){
  for(xmlAttrPtr attr=node->properties;NULL!=attr;attr=attr->next){
    xmlChar*attrname=xmlNodeListGetString(node->doc,attr->children,1);
    if(!xmlStrcmp(dbid_attr_value,attrname)){
      xmlFree(attrname);
      return true;
    }
  }
  return false;
}
// get language from tuv attribute
string getLanAttr(xmlNodePtr node){
  for(xmlAttrPtr attr=node->properties;NULL!=attr;attr=attr->next){
    xmlChar*attrname=xmlNodeListGetString(node->doc,attr->children,1);
    for(int i=0;i<lans.size();++i){
      if(!xmlStrcmp(lans[i],attrname)){
        string ret{string((char*)lans[i])};
        xmlFree(attrname);
        return ret;
      }
    }
    xmlFree(attrname);
  }
  // we did not find the language
  throw runtime_error("missing language code in tuv");
}
// We assume the file is correctly formatted so we can rely on our state
// (if we receive a tag we should not receive in the current state, then we toss an exception)
static void processNode(xmlTextReaderPtr reader){
  const xmlChar*name,*value;
  bool opening;

  // get name of target together with value, and open/close tag
  name=xmlTextReaderConstName(reader);
  value=xmlTextReaderConstValue(reader);
  opening=xmlTextReaderNodeType(reader)==1;

  // hget current node
  xmlNodePtr node=xmlTextReaderCurrentNode(reader);

  // process state transitions and get segments
  switch(state){
    case NOT_STARTED:
      if(opening&&!xmlStrcmp(name,header_tag)){
        state=HEADER;
      }
      break;
    case HEADER:
      if(opening&&!xmlStrcmp(name,tu_tag)){
        state=TU;
      }else
      if(opening&&!xmlStrcmp(name,prop_tag)&&tagHasDbid(node)){
        getdbidFromHeader=true;
      }else
      if(getdbidFromHeader&&!xmlStrcmp(name,text_tag)){
        getdbidFromHeader=false;
        printf("DBID: %s\n",value);
      }
      break;
    case TU:
      if(opening&&!xmlStrcmp(name,tuv_tag)){
        currLan=getLanAttr(node);
        state=TUV;
       }
     break;
    case TUV:
      if(opening&&!xmlStrcmp(name,seg_tag)){
        state=SEG;
      }else
      if(!opening&&!xmlStrcmp(name,tu_tag)){
        state=TU;
      }else
      if(opening&&!xmlStrcmp(name,tuv_tag)){
        currLan=getLanAttr(node);
        state=TUV;
       }
      break;
    case SEG:
      if(!opening&&!xmlStrcmp(name,tu_tag)){
        state=TU;
      }else
      if(!opening&&!xmlStrcmp(name,tuv_tag)){
        state=TUV;
      }else
      if(!opening&&!xmlStrcmp(name,seg_tag)){
        // we now have the complete segment
        boost::algorithm::trim(currseg);
        boost::algorithm::trim_right(currseg);
        printf("LAN: %s, SEG: %s\n",currLan.c_str(),currseg.c_str());
        currseg="";
      }else
      if(!xmlStrcmp(name,text_tag)){
        // we might get tags inside the segment - append text as long as we are inside the seg tag
        currseg+=string((char*)value);
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
