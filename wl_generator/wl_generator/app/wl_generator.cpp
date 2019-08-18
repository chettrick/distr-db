// wl_generator.cpp
// Used to mimick front end of CSC462 front-end when 
// stress testing the system

#include <iostream>
#include <functional>
#include <thread>
#include "ra/queue.hpp"
#include "ra/thread_pool.hpp"

using namespace std;
     
int main()
{
  cout << "Hello newworld!" << endl;
  std::function<void()> fun = []() { std::cout << "running lambda function" << std::endl; };
  std::thread t1(fun);
  t1.join();

  using function_type = std::function<void()>;
  const std::size_t QUEUE_SIZE = 500;
//  ra::concurrency::queue<function_type> q(QUEUE_SIZE);
  ra::concurrency::thread_pool tp;
  std::cout << "finished tester, queue and threadpool made" << std::endl;
  return 0;
}
