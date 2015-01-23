// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015. All rights reserved.

#include "bpf_queue/Utils.h"
#include "bpf_queue/Message.h"
#include "bpf_queue/Queue.h"
#include <boost/log/trivial.hpp>
#include <sstream>
using namespace std;
namespace bpf{

// listen on a queue and call a callback function when message is received
shared_ptr<Message>listen(std::shared_ptr<Queue>q,function<void(shared_ptr<Message>)>cb){
  while(true){
    shared_ptr<Message>msg{q->deq(true)};
    try{
      // call client callback function
      cb(msg);
    }
    catch(exception const&e){
      BOOST_LOG_TRIVIAL(warning)<<"bpf::listen(): caught client exception: \""<<e.what()<<"\"";
    }
  }
}
// serialize a message
string serializeXmlMessage(shared_ptr<Message>msg){
  stringstream str;
  boost::archive::xml_oarchive oar(str);
  oar<<boost::serialization::make_nvp("msg",*msg);
  return str.str();
}
// de-serialize xml message
shared_ptr<Message>deserializeXmlMessage(istream&is){
  boost::archive::xml_iarchive iar(is);
  shared_ptr<Message>msg{shared_ptr<Message>(new Message())};
  iar>>boost::serialization::make_nvp("msg",*msg);
  return msg;
}
}
