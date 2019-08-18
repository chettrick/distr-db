/*
 * thread_pool.cpp
 * Dusty Miller
 * V00160288
 * SENG475 - Summer 2019
 * Assignment 6: cpp_concurrency
 */

#include "ra/thread_pool.hpp"
#include <functional>
#include <iostream>

namespace ra::concurrency {

	// Thread pool class implementation.
	// Note: queue (jobs) is created at an arbitrary size of 32 currently
	thread_pool::thread_pool() : jobs(queue<std::function<void()>>(32)), shutting_down(false)
	{
		// create each thread in threads in-place and start them running
		// on function do_work.  This is safe since the queue has already
		// been created via initializer list
		std::function<void()> f = [&](){ do_work(); };

		// if hardware_concurrency() == 0, set num_threads = 2, 
		// else set num_threads = hardware_concurrency
		const int DEFAULT_NUM_THREADS = 2;
		size_type num_threads = (std::thread::hardware_concurrency() ? std::thread::hardware_concurrency() : DEFAULT_NUM_THREADS);
		for(size_type i = 0; i < num_threads; ++i) {
			threads.emplace_back(std::thread(f));
		}
	}

	thread_pool::thread_pool(size_type num_threads):
		jobs(queue<std::function<void()>>(32)), shutting_down(false)

	{
		// lambda function - this is what the threads will be executing
		std::function<void()> f = [&](){ do_work(); };
		for(size_type i = 0; i < num_threads; ++i) {
			threads.emplace_back(std::thread(f));
		}
	}

	thread_pool::~thread_pool() {
		shutdown();
	}

	std::size_t thread_pool::size() const {
		return threads.size();
	}

	void thread_pool::schedule(std::function<void()>&& func)
	{
		jobs.push(std::move(func));
	}

	void thread_pool::shutdown()
	{
		std::cout << "Running shutdown in thread_pool" << std::endl;
		shutting_down = true;
		jobs.close();
		for(size_type i = 0; i < threads.size(); ++i) {
			threads[i].join();
		}
	}

	// private function, executed by each thread
	// in thread_pool
	void thread_pool::do_work() {
		queue_function_type::status cur_status;
		std::function<void()> cur_job;
		do {
			cur_status = jobs.pop(cur_job);
			if(cur_status == queue_function_type::status::success) {
				cur_job();
			}
		} while(cur_status == queue_function_type::status::success);
	}

	bool thread_pool::is_shutdown() const
	{
		return shutting_down;
	}
}
