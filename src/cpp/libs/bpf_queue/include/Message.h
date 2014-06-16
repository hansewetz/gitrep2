#ifndef __MESSAGE_H__
#define __MESSAGE_H__
#include "bpf_utils/Id.h"
#include <iosfwd>

// serialization stuff
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/serialization/string.hpp>

namespace bpf{

// identifier type for class
class Message;
using MessageId=Id<Message,std::string,true>;

// message used for communication
class Message{
public:
  // print operator
  friend std::ostream&operator<<(std::ostream&,Message const&);

  // classification of message
  enum class msgtype_t:int{REQUEST=0,REPLY=1};

  // ctors,dtor assign (non copyable but movable)
  Message(msgtype_t);
  Message(Message const&)=delete;
  Message(Message&&)=default;
  Message&operator=(Message const&)=delete;
  Message&operator=(Message&&)=default;
  ~Message()=default;

  // print function
  std::ostream&print(std::ostream&os)const;

  // getters
  msgtype_t msgtype()const;
  MessageId const&id()const;
private:
  MessageId id_;
  msgtype_t msgtype_;

  // serialization stuff
  Message()=default;
  friend std::shared_ptr<Message>deserializeXmlMessage(std::istream&is);
  friend class boost::serialization::access;
  template<typename Archive>friend void serialize(Archive&ar,Message&pi,const unsigned int version);
};
// serialize/deserialize class into/from boost archive
template <typename Archive>
void serialize(Archive&ar,Message&msg,const unsigned int version){
  using boost::serialization::make_nvp;
  ar&make_nvp("msg",msg.id_);
  ar&make_nvp("msg",msg.msgtype_);
}
}
#endif
