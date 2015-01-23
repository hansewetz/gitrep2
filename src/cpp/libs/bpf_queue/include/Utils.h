// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015. All rights reserved.

#ifndef __UTILS_H__
#define __UTILS_H__
#include <functional>
#include <memory>
namespace bpf{
// forward decl.
class Message;
class Queue;

// listen on a queue and call a callback function when message is received
std::shared_ptr<Message>listen(std::shared_ptr<Queue>q,std::function<void(std::shared_ptr<Message>)>cb);

// Message (de)-serialization
std::string serializeXmlMessage(std::shared_ptr<Message>msg);
std::shared_ptr<Message>deserializeXmlMessage(std::istream&is);
}
#endif
