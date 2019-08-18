/*
 * test_thread_pool.cpp
 * Dusty Miller
 * V00160288
 * SENG475 - Summer 2019
 * Assignment 6: cpp_concurrency
 */

//#define CATCH_CONFIG_MAIN // this tells catch to provide a main()

//#include <catch2/catch.hpp>
#include <iostream>
#include <string>
#include <thread>
#include <functional>
#include "ra/thread_pool.hpp"
#include <chrono> // for timeouts

using size_type = std::size_t;
int main() {
	std::cout << "in tester" << std::endl;
	std::function<void()> task1 = [](){std::cout << "doing task1" << std::endl;};

	ra::concurrency::thread_pool tp1;
	ra::concurrency::thread_pool tp2(2);

	std::function<void()> job = std::function([]() {std::cout << "doing 'job'" << std::endl;});
	for(size_type i = 0; i < 1000; ++i) {
		tp1.schedule(std::move(job));
		tp2.schedule(std::move(job));

	}
	
	std::this_thread::sleep_for(std::chrono::seconds(1));

}
/**************************************
TEMPLATE_TEST_CASE("queue class", "int", int) {

	ra::concurrency::thread_pool tp1;
//	ra::concurrency::thread_pool tp2(16);

	std::cout << "std::thread::hardware_concurrency(): " << std::thread::hardware_concurrency() << std::endl;

//	CHECK(tp1.size() == std::thread::hardware_concurrency());
//	CHECK(tp2.size() == 16);
}
*********************************************/
