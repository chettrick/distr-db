#include <iostream>
#include <functional>
#include <thread>
#include <SFML/Network.hpp>

using namespace std;
     
int main()
{
	std::cout << "included SFML package" << std::endl;
  std::function<void()> fun = []() { std::cout << "running lambda function" << std::endl; };
  std::thread t1(fun);
  t1.join();
  return 0;
}
