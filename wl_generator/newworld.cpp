#include <iostream>
#include <functional>
#include <thread>

using namespace std;
     
int main()
{
  cout << "Hello newworld!" << endl;
  std::function<void()> fun = []() { std::cout << "running lambda function" << std::endl; };
  std::thread t1(fun);
  t1.join();
  return 0;
}
