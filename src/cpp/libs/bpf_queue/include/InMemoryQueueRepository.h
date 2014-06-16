#ifndef __IN_MEMORY_QUEUE_REP_H__
#define __IN_MEMORY_QUEUE_REP_H__
#include "bpf_queue/QueueRepository.h"
#include "bpf_utils/utility.h"
#include <iosfwd>
#include <memory>
#include <unordered_map>
namespace bpf{

// repository for queue definition
class InMemoryQueueRepository:public QueueRepository{
public:
  // ctors, assign,dtor (non copyabable/movable)
  using QueueRepository::QueueRepository;
  virtual ~InMemoryQueueRepository()=default;

  // print function
  virtual std::ostream&print(std::ostream&)const override;

  // getters 
  virtual bool hasQueue(QueueId const&)const override;
  virtual std::shared_ptr<Queue>queue(QueueId const&)const override;

  // setters
  virtual void addQueue(std::shared_ptr<Queue>)override;
  virtual void rmQueue(QueueId const&)override;
private:
  std::unordered_map<QueueId,std::shared_ptr<Queue>,hash<QueueId>>queues_;
};
}
#endif
