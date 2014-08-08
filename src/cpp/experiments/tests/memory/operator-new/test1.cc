#include <iostream>
#include <cstdlib>
#include <string>
#include <new>
using namespace std;

// memory pool 
class MemPool{
public:
  MemPool()=default;
  MemPool(MemPool const&)=delete;
  MemPool(MemPool&&)=delete;
  MemPool*operator=(MemPool const&)=delete;
  MemPool*operator=(MemPool&&)=delete;
  ~MemPool()=default;

  // allocate memory
  void*alloc(size_t size){
    return malloc(size);
  }
  // deallocate memory
  void deallocate(void*mem){
    free(mem);
  }
};
// memory allocator
void*operator new(size_t size,MemPool*pool){
  return pool->alloc(size);
}
// deallocator
void operator delete(void*mem,MemPool*pool){
  pool->deallocate(mem);
}

// test program
int main(){
  // construct a string using a memory pool using placement new
  MemPool mp1;
  string*s{new(&mp1)string("Hello world")};
  cout<<"string: "<<*s<<endl;

  // destruct string and free memory (cannot be done using simple delete)
  s->~string();
  operator delete(s,&mp1);
}

