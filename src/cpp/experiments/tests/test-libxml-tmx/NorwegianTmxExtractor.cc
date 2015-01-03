// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014. All rights reserved.

#include "NorwegianTmxExtractor.h"
#include <iostream>
#include <string>
#include <stdexcept>
#include <vector>
#include <stdexcept>
#include <stdio.h>
#include <string.h>
#include <libxml/xmlreader.h>
#include <boost/algorithm/string.hpp>
using namespace std;
namespace mtdata{

// helper class for holding segments
string const&NorwegianTmxExtractor::Segment::srclan()const{return srclan_;}
string const&NorwegianTmxExtractor::Segment::targetlan()const{return targetlan_;}
string const&NorwegianTmxExtractor::Segment::srcseg()const{return srcseg_;}
string const&NorwegianTmxExtractor::Segment::targetseg()const{return targetseg_;}

// print debug operator for Segment
ostream&operator<<(ostream&os,NorwegianTmxExtractor::Segment const&s){
  return os<<"["<<s.srclan()<<"]["<<s.targetlan()<<"]["<<s.srcseg()<<"]["<<s.targetseg()<<"]";
}
// check that a segment is OK
void NorwegianTmxExtractor::Segment::checkSeg()const{
  // check languages
  if(srclan_!="en")throw runtime_error(string("invalid source language: \"")+srclan_+"\"");
  if(targetlan_!="nb"&&targetlan_!="nn")throw runtime_error(string("invalid target language: \"")+targetlan_+"\"");
  if(srcseg_=="")throw runtime_error(string("invalid source segment: \"")+srcseg_+"\"");
  if(targetseg_=="")throw runtime_error(string("invalid target segment: \"")+targetseg_+"\"");
}
// static data and functions
namespace{
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
  vector<const xmlChar*>lans{(unsigned char*)"nb-NO",(unsigned char*)"nn-NO",(unsigned char*)"en-GB"};

  // helper function: check if tag contains a specific attribute
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
  // helper function: get language from tuv attribute
  string getLanAttr(xmlNodePtr node){
    for(xmlAttrPtr attr=node->properties;NULL!=attr;attr=attr->next){
      xmlChar*attrname=xmlNodeListGetString(node->doc,attr->children,1);
      for(std::size_t i=0;i<lans.size();++i){
        if(!xmlStrcmp(lans[i],attrname)){
          string ret{string((char*)lans[i])};
          ret=ret.substr(0,2);
          xmlFree(attrname);
          return ret;
        }
      }
      xmlFree(attrname);
    }
    // we did not find the language
    throw runtime_error("missing language code in tuv");
  }
}
// ctor
NorwegianTmxExtractor::NorwegianTmxExtractor(boost::filesystem::path const&tmxpath){
  // check if we could open file for xl parsing using libxml
  xmlTextReaderPtr reader{(xmlReaderForFile(tmxpath.string().c_str(),NULL,0))};
  if(reader==NULL){
    throw runtime_error(string("could not open file: ")+tmxpath.string()+" for libxml parsing");
  }
  // scan entire file - toss exception if error
  int ret=xmlTextReaderRead(reader);
  while (ret==1) {
    processNode(reader);
    ret=xmlTextReaderRead(reader);
  }
  xmlFreeTextReader(reader);
  if (ret!=0){
    runtime_error(string("failed to parse tmx file: ")+tmxpath.string()+" during libxml parsing");
  }
}
// getters
string NorwegianTmxExtractor::dbid()const{
  return dbid_;
}
vector<NorwegianTmxExtractor::Segment>NorwegianTmxExtractor::segs()const{
  return segs_;
}


// process node (called repreatadly)
void NorwegianTmxExtractor::processNode(xmlTextReaderPtr reader){
  const xmlChar*name,*value;
  bool opening;

  // get name of target together with value, and open/close tag
  name=xmlTextReaderConstName(reader);
  value=xmlTextReaderConstValue(reader);
  opening=xmlTextReaderNodeType(reader)==1;

  // hget current node
  xmlNodePtr node=xmlTextReaderCurrentNode(reader);

  // process state transitions and get segments
  switch(state_){
    case NOT_STARTED:
      if(opening&&!xmlStrcmp(name,header_tag)){
        state_=HEADER;
      }
      break;
    case HEADER:
      if(opening&&!xmlStrcmp(name,tu_tag)){
        state_=TU;
      }else
      if(opening&&!xmlStrcmp(name,prop_tag)&&tagHasDbid(node)){
        getdbidFromHeader_=true;
      }else
      if(getdbidFromHeader_&&!xmlStrcmp(name,text_tag)){
        getdbidFromHeader_=false;
        dbid_=string{(char*)value};
      }
      break;
    case TU:
      if(opening&&!xmlStrcmp(name,tuv_tag)){
        currlan_=getLanAttr(node);
        state_=TUV;
        seg_=Segment{};
       }
     break;
    case TUV:
      if(opening&&!xmlStrcmp(name,seg_tag)){
        state_=SEG;
      }else
      if(!opening&&!xmlStrcmp(name,tu_tag)){
        // save segment
        seg_.checkSeg();
        segs_.push_back(seg_);
        seg_=Segment();
        state_=TU;
      }else
      if(opening&&!xmlStrcmp(name,tuv_tag)){
        currlan_=getLanAttr(node);
        state_=TUV;
       }
      break;
    case SEG:
      if(!opening&&!xmlStrcmp(name,tu_tag)){
        state_=TU;
      }else
      if(!opening&&!xmlStrcmp(name,tuv_tag)){
        state_=TUV;
      }else
      if(!opening&&!xmlStrcmp(name,seg_tag)){
        boost::algorithm::trim(currseg_);
        boost::algorithm::trim_right(currseg_);

        // save language and segment in segment we are building
        if(currlan_=="en"){
          seg_.srclan_=currlan_;
          seg_.srcseg_=currseg_;
        }else{
          seg_.targetlan_=currlan_;
          seg_.targetseg_=currseg_;
        }
        // we are done with one segment
        currlan_="";
        currseg_="";
      }else
      if(!xmlStrcmp(name,text_tag)){
        // we might get tags inside the segment - append text as long as we are inside the seg tag
        currseg_+=string((char*)value);
      }
      break;
  }
}
}
