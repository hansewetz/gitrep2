// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014. All rights reserved.

#include "bpf_queue/Message.h"
#include <iostream>
using namespace std;
namespace bpf{

// ctor
Message::Message(msgtype_t msgtype):msgtype_(msgtype),id_(){
}
// get message type
Message::msgtype_t Message::msgtype()const{
  return msgtype_;
}
// get message id
MessageId const&Message::id()const{
  return id_;
}
// print function
ostream&Message::print(ostream&os)const{
  return os<<"msgtype_: "<<(msgtype_==msgtype_t::REQUEST?"REQUEST":"REPLY")<<", id_: ["<<id_<<"]";
}
// print operator
ostream&operator<<(ostream&os,Message const&m){
  return m.print(os);
}
}
