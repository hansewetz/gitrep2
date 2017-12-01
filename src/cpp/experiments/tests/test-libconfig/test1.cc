#include "libconfig.h++"
#include <boost/lexical_cast.hpp>
#include <string>
#include <iostream>

using namespace std;
using namespace libconfig;

// convert a setting to a string
string setting2string(Setting const&s){
  if(s.getType()==Setting::Type::TypeString){
    return string(s);
  }else
  if(s.getType()==Setting::Type::TypeInt){
    return boost::lexical_cast<string>(int(s));
  }else
  if(s.getType()==Setting::Type::TypeBoolean){
    return boost::lexical_cast<string>(bool(s));
  }else
  if(s.getType()==Setting::Type::TypeInt64){
    return boost::lexical_cast<string>(long(s));
  }else
  if(s.getType()==Setting::Type::TypeFloat){
    return boost::lexical_cast<string>(float(s));
  }
  throw std::runtime_error("cannot convert Setting to string (Stting must be a simple type)");
}
// resolve a variable - if the value is in the form '${xxx}' then pick up an environment variable
// (should probably also support references to variables inside the config file)
// NOTE! should be implemented using proper tokanization + appending tokens
char nextchar(size_t&ind,string const&value){
  if(ind>=value.size())return 0;
  return value[ind++];
}
string resolve(string const&value){
  string ret;
  size_t ind=0;
  char c;
  while((c=nextchar(ind,value))!=0){
    if(c=='$'){
      c=nextchar(ind,value);
      if(c!='{')throw runtime_error("'$' must be followed by a '{'");
      char sbuf[1024-1];
      char*p=&sbuf[0];
      c=nextchar(ind,value);
      if(!isalpha(c))throw runtime_error("a '${' must be followed by an alpha character");
      do{
        if(p>=sbuf+sizeof(sbuf)-1)throw runtime_error("environment variable  too long");
        *p++=c;
      }while((c=nextchar(ind,value))!=0&&(isalnum(c)||c=='_'));
      *p='\0';
      if(c!='}')throw runtime_error("an environment variable must end with a '}'");
      char*penv=getenv(sbuf);
      if(!penv)throw runtime_error("environment variable: "s+sbuf+" is not set inenvironment");
      ret+=penv;
    }else{
      ret+=c;
    }
  }
  return ret;
}
// test program
int main(){
  Config cfg;
  try{
    // open config file
    cfg.readFile("test.cfg");

    // read src+trg languages
    string srclan;
    bool prod;
    if(cfg.lookupValue("smtconfig.training.srclan",srclan)&&cfg.lookupValue("smtconfig.training.prod",prod)){
      cout<<"srclan: "<<srclan<<boolalpha<<", prod: "<<prod<<endl;
    }else{
      cout<<"could not find srclan and/or prod"<<endl;
    }
    // print all properties under 'smtconfig.training'
    Setting const&root=cfg.getRoot();
    Setting const&tdata=root["smtconfig"]["training"];
   for(auto const&data:tdata){
      cout<<"name: "<<data.getName()<<", value: "<<resolve(setting2string(data))<<endl;
    }
  }
  catch(ParseException const&pex){
    cerr<<"parse exception: "<<pex.getFile()<<":"<<pex.getLine()<<" - "<<pex.getError()<<endl;
  }
  catch(std::exception const&e){
    cerr<<"exception: "<<e.what()<<endl;
  }
  return 0;
}
