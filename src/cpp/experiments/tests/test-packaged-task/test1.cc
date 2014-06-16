#include <iostream>
#include <exception>
#include <fstream>
#include <list>
#include <set>
#include <thread>
#include <mutex>
#include <iterator>
#include <vector>
#include <future>
#include <functional>
#include <algorithm>
#include <random>
#include <chrono>
#include <map>
#include <utility>
#include <unistd.h>
#include <type_traits>
#include <chrono>
#include <ratio>

using namespace std;

template<typename F>
std::future<typename std::result_of<F()>::type>submitTask(F f,std::thread&t){
  typedef typename std::result_of<F()>::type res_t;
  std::packaged_task<res_t()>task(std::move(f));
  std::future<res_t>fut=task.get_future();
  t=std::thread(std::move(task));
  return fut;
}
// task function
int getRandomNumber(){
  std::random_device generator;
  std::uniform_int_distribution<int>distr(0,99);
  return distr(generator);
}

int main(){
  thread t;
  std::future<int>fut=submitTask(getRandomNumber,t);
  cerr<<"task generated random number: "<<fut.get()<<endl;
  t.join();
}
