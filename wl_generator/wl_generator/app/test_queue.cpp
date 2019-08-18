/*
 * test_queue.cpp
 * Dusty Miller
 * V00160288
 * SENG475 - Summer 2019
 * Assignment 6: cpp_concurrency
 */

#define CATCH_CONFIG_MAIN // this tells catch to provide a main()

#include <catch2/catch.hpp>
#include <iostream>
#include <string>
#include <thread>
#include "ra/queue.hpp"
#include <chrono> // for timeouts

using size_type = std::size_t;

void print_test_label(std::string test_name) {
	auto underscore = "*******************************************\n";
	std::cout << "\n\n" << underscore << test_name << "\n" << underscore;
}
template <class T>
auto make_push_fun() {
	return [](ra::concurrency::queue<T> *q_ptr, size_type n) 
	{
		for(size_type i = 0; i < n; ++i) {
		std::cout << "push i,n: " << i << "," << n << std::endl;
		q_ptr->push(T(i));
		}	
	};
}

template <class T>
auto make_pop_fun() {
	return [](ra::concurrency::queue<T> *q_ptr, size_type n) {
		std::cout << "pop, n: " << n << std::endl;

		for(size_type i = 0; i < n; ++i) {
		T result;
		q_ptr->pop(result);
		CHECK(result == T(i));
		std::cout << "pop count: " << i << std::endl;
	  }	};
}

// Basic test, push to a queue until it is over-full, after a 1s delay, create a 2nd thead that begins popping from queue (allowing 1st thread to finish pushing).
template <class T>
void test_fun_1(std::function<void (ra::concurrency::queue<T>*, size_type)> push_fun, std::function<void (ra::concurrency::queue<T>*, size_type)> pop_fun)
{
	const size_type QUEUE_SIZE = 10;
	ra::concurrency::queue<T> q(QUEUE_SIZE);
	CHECK(q.max_size() == QUEUE_SIZE);
	std::thread t1(push_fun, &q, 15);
	std::this_thread::sleep_for(std::chrono::seconds(1));
	CHECK(q.is_full());
	CHECK(!q.is_empty());
	std::thread t2(pop_fun, &q, 15);
	t1.join();
	t2.join();
	CHECK(!q.is_full());
	CHECK(q.is_empty());
}

// Basic test, a thread is attempting to pop more values than exist on the queue (then more values are pushed to the queue later).
template <class T>
void test_fun_2(std::function<void (ra::concurrency::queue<T>*, size_type)> push_fun, std::function<void (ra::concurrency::queue<T>*, size_type)> pop_fun)
{
	const size_type QUEUE_SIZE = 10;
	ra::concurrency::queue<T> q(QUEUE_SIZE);
	CHECK(q.max_size() == QUEUE_SIZE);
	std::thread t1(push_fun, &q, 5);
	std::thread t2(pop_fun, &q, 5);
	std::thread t3(pop_fun, &q, 20);
	std::this_thread::sleep_for(std::chrono::seconds(1));
	std::thread t4(push_fun, &q, 20);
	t1.join();
	t2.join();
	t3.join();
	t4.join();
}

// Testing the close function.  Ensures push/pop returns closed status on an empty and closed queue.
template <class T>
void test_fun_3(std::function<void (ra::concurrency::queue<T>*, size_type)> push_fun, std::function<void (ra::concurrency::queue<T>*, size_type)> pop_fun)
{
	const size_type QUEUE_SIZE = 10;
	ra::concurrency::queue<T> q(QUEUE_SIZE);
	CHECK(q.max_size() == QUEUE_SIZE);
	q.push(T(0));
	std::this_thread::sleep_for(std::chrono::seconds(1));
	q.close();
	auto result_status = q.push(T(1));
	CHECK(result_status == ra::concurrency::queue<T>::status::closed);
	T result_value;
	result_status = q.pop(result_value);
	CHECK(result_value == T(0));
	CHECK(result_status == ra::concurrency::queue<T>::status::success);

	result_status = q.pop(result_value);
	CHECK(result_status == ra::concurrency::queue<T>::status::closed);
}

template <class T>
void test_fun_4(std::function<void (ra::concurrency::queue<T>*, size_type)> push_fun, std::function<void (ra::concurrency::queue<T>*, size_type)> pop_fun)
{
	const size_type QUEUE_SIZE = 1;
	ra::concurrency::queue<T> q(QUEUE_SIZE);
	CHECK(q.max_size() == QUEUE_SIZE);

	std::thread t1(push_fun, &q, 1);
	std::this_thread::sleep_for(std::chrono::seconds(1));
	
	// lambda function that pushes a single value to the queue, 
	// it asserts that the value returned is equal to status::closed
	auto push_and_check_closed = [&q]() {
		auto result_status = q.push(T(1));
		CHECK(result_status == ra::concurrency::queue<T>::status::closed);
	};

	std::thread t2(push_and_check_closed);
	std::thread t3(push_and_check_closed);
	std::thread t4(push_and_check_closed);

	std::this_thread::sleep_for(std::chrono::seconds(1));
	q.close();
	t1.join();
	t2.join();
	t3.join();
	t4.join();
	T result_value;
	auto result_status = q.pop(result_value);
	CHECK(result_status == ra::concurrency::queue<T>::status::success);
	CHECK(result_value == 0);
}

// test_fun_5 will ensure that elements can still be popped from the queue when it is
// closed, and that only when there are no more elements remaining is the return
// value status::closed
template <class T>
void test_fun_5(std::function<void (ra::concurrency::queue<T>*, size_type)> push_fun, std::function<void (ra::concurrency::queue<T>*, size_type)> pop_fun)
{
	const size_type QUEUE_SIZE = 5;
	ra::concurrency::queue<T> q(QUEUE_SIZE);
	CHECK(q.max_size() == QUEUE_SIZE);

	// lambda function pushes 5 elements on queue, check status == success
	auto push_and_check_success = [&q]() {
		std::cout << "starting push_and_check_success" << std::endl;
		for(size_type i = 0; i < 5; ++i) {
			auto result_status = q.push(T(i));
			CHECK(result_status == ra::concurrency::queue<T>::status::success);
		}
		std::cout << "done push_and_check success" << std::endl;
	};

	auto pop_and_confirm_success = [&q]() {
		for(size_type i = 0; i < 5; ++i) {
			std::cout << "pop and confirm success" << std::endl;
			T result_value;
			auto result_status = q.pop(result_value);
			CHECK(result_status == ra::concurrency::queue<T>::status::success);
		}
	};
		
	auto pop_and_confirm_closed = [&q]() {
		T result_value;
		auto result_status = q.pop(result_value);
		CHECK(result_status == ra::concurrency::queue<T>::status::closed);
	};
		
	std::thread t1(push_and_check_success);
	t1.join();
	q.close();
	std::this_thread::sleep_for(std::chrono::seconds(1));
	std::thread t2(pop_and_confirm_success);
	std::this_thread::sleep_for(std::chrono::seconds(1));
	std::thread t3(pop_and_confirm_closed);
	t2.join();
	t3.join();
}

// Testing clear function in queue
template <class T>
void test_fun_6(std::function<void (ra::concurrency::queue<T>*, size_type)> push_fun, std::function<void (ra::concurrency::queue<T>*, size_type)> pop_fun)
{
	// make queue
	const size_type QUEUE_SIZE = 5;
	ra::concurrency::queue<T> q(QUEUE_SIZE);

	// add elements
	for(size_type i = 0; i < 5; ++i) {
		q.push(T(i));
	}

	// confirm queue is not empty
	CHECK(!q.is_empty());
	// call clear
	q.clear();
	// confirm queue is empty
	CHECK(q.is_empty());
	// push more elements to queue
	for(size_type i = 0; i < 5; ++i) {
		q.push(T(i));
	}

	// leave scope - this will trigger a memory leak if clear isn't
	// automatically called by destructor
}

TEMPLATE_TEST_CASE("queue class", "int, long, float, double", int, long, float, double) {
		// call factory functions to create some
		// universial lambda functions used througout testing
		auto f_push = make_push_fun<TestType>();
		auto f_pop = make_pop_fun<TestType>();

		// call each individual test function.
		print_test_label("Test 1");
		test_fun_1<TestType>(f_push, f_pop);
		print_test_label("Test 2");
		test_fun_2<TestType>(f_push, f_pop);
		print_test_label("Test 3");
		test_fun_3<TestType>(f_push, f_pop);
		print_test_label("Test 4");
		test_fun_4<TestType>(f_push, f_pop);
		print_test_label("Test 5");
		test_fun_5<TestType>(f_push, f_pop);
}
