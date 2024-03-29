/*
 * thread_pool.hpp
 * Dusty Miller
 * V00160288
 * SENG475 - Summer 2019
 * Assignment 6: cpp_concurrency
 */

#ifndef THREAD_POOL_HPP
#define THREAD_POOL_HPP

#include "ra/queue.hpp"
#include <thread>
#include <functional>

namespace ra::concurrency {

	using queue_function_type = queue<std::function<void()> >;
	// Thread pool class definition.
	class thread_pool
	 {
	 public:

		 // An unsigned integral type used to represent sizes.
		 using size_type = std::size_t;

		// Creates a thread pool with the number of threads equal to the
		// hardware concurrency level (if known); otherwise the number of
		// threads is set to 2.
		thread_pool();

		// Creates a thread pool with num_threads threads.
		// Precondition: num_threads > 0
		thread_pool(std::size_t num_threads);

		// A thread pool is not copyable or movable.
		thread_pool(const thread_pool&) = delete;
		thread_pool& operator=(const thread_pool&) = delete;
		thread_pool(thread_pool&&) = delete;
		thread_pool& operator=(thread_pool&&) = delete;

		// Destroys a thread pool, shutting down the thread pool first
		// (if not already shutdown).
		~thread_pool();

		// Gets the number of threads in the thread pool.
		// This function is not thread safe.
		size_type size() const;

		// Enqueues a task for execution by the thread pool.
		// This function inserts the task specified by the callable
		// entity func into the queue of tasks associated with the
		// thread pool.
		// This function may block if the number of currently
		// queued tasks is sufficiently large.
		// Note: The rvalue reference parameter is intentional and
		// implies that the schedule function is permitted to change
		// the value of func (e.g., by moving from func).
		// Precondition: The thread pool is not in the shutdown state
		// and is not currently in the process of being shutdown via
		// the shutdown member function.
		// This function is thread safe.
		void schedule(std::function<void()>&& func);

		// Shuts down the thread pool.
		// This function places the thread pool into a state where
		// new tasks will no longer be accepted via the schedule
		// member function.
		// Then, the function blocks until all queued tasks
		// have been executed and all threads in the thread pool
		// are idle (i.e., not currently executing a task).
		// Finally, the thread pool is placed in the shutdown state.
		// If the thread pool is already shutdown at the time that this
		// function is called, this function has no effect.
		// After the thread pool is shutdown, it can only be destroyed.
		// This function is thread safe.

		void shutdown();

		// Tests if the thread pool has been shutdown.
		// This function is not thread safe.
		bool is_shutdown() const;

	 private:
		queue<std::function<void()>> jobs;
		std::vector<std::thread> threads;
		void do_work(); // function executed by each thread in threads
		bool shutting_down;
	};
}

#endif
