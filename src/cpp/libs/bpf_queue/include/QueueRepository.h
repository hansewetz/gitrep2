// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015. All rights reserved.

#ifndef __QUEUE_REPOSITORY_H__
#define __QUEUE_REPOSITORY_H__
#include "bpf_queue/Queue.h"
#include <iosfwd>
#include <memory>
namespace bpf{

// identifier type for class
class QueueRepository;
using QueueRepositoryId=utils::Id<QueueRepository>;

// repository for queue definition
class QueueRepository{
public:
  // print function
  friend std::ostream&operator<<(std::ostream&,QueueRepository const&);

  // ctors, assign,dtor (non copyabable/movable)
  QueueRepository(QueueRepositoryId const&);
  QueueRepository(QueueRepository const&)=delete;
  QueueRepository(QueueRepository&&)=delete;
  QueueRepository&operator=(QueueRepository const&)=delete;
  QueueRepository&operator=(QueueRepository&&)=delete;
  virtual ~QueueRepository()=default;

  // print function
  virtual std::ostream&print(std::ostream&)const=0;

  // getters
  QueueRepositoryId const&id()const;

  // pure virtual getters (depends on derived class)
  virtual bool hasQueue(QueueId const&)const=0;
  virtual std::shared_ptr<Queue>queue(QueueId const&)const=0;

  // pure virtual setters
  virtual void addQueue(std::shared_ptr<Queue>)=0;
  virtual void rmQueue(QueueId const&)=0;
private:
  QueueRepositoryId id_;
};
}
#endif
