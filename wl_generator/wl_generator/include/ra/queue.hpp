/*
 * queue.hpp
 * Dusty Miller
 * V00160288
 * SENG475 - Summer 2019
 * Assignment 6: cpp_concurrency
 */

#ifndef QUEUE_HPP
#define QUEUE_HPP

#include <iostream>
#include <queue>
#include <list>
#include <mutex>
#include <condition_variable>

namespace ra {
namespace concurrency {

	// Concurrency bounded FIFO queue class.
	template <class T>
	class queue
	{
	public:

		// The type of each of the elements stored in the queue.
		using value_type = T;

		// An unsigned integral type used to represent sizes.
		using size_type = std::size_t;

		// A type for the status of a queue operation.
		enum class status {
			success = 0, // operation successful
			empty, // queue is empty (not currently used)
			full, // queue is full (not currently used)
			closed, // queue is closed
		};

		// A queue is not default constructible.
		queue() = delete;

		// Constructs a queue with a maximum size of max_size.
		// The queue is marked as open (i.e., not closed).
		// Precondition: The quantity max_size must be greater than
		// zero.
		queue(size_type max_size)
		{
			queue_ = new std::queue<value_type, std::list<value_type> >[max_size];
			capacity_ = max_size;
			is_closed_ = false; // marking queue as open
		}

		// A queue is not movable or copyable.
		queue(const queue&) = delete;
		queue& operator=(const queue&) = delete;
		queue(queue&&) = delete;
		queue& operator=(queue&&) = delete;

		// Destroys the queue after closing the queue (if not already
		// closed) and clearing the queue (if not already empty).
		~queue()
		{
			if(!is_empty()) {
				clear();
			}
			delete[] queue_;
			std::cout << "queue destructor running..." << std::endl;
		}

		// Inserts the value x at the end of the queue, blocking if
		// necessary.
		// If the queue is full, the thread will be blocked until the
		// queue insertion can be completed or the queue is closed.
		// If the value x is successfully inserted on the queue, the
		// function returns status::success.
		// If the value x cannot be inserted on the queue (due to the
		// queue being closed), the function returns with a return
		// value of status::closed.
		// This function is thread safe.
		// Note: The rvalue reference parameter is intentional and
		// implies that the push function is permitted to change
		// the value of x (e.g., by moving from x).
		status push(value_type&& x)
		{
		// TODO: allow threads blocked  here to wake up
		// when queue is closed (modify the lambda function)
			// lock queue and status
			std::unique_lock lock(queue_mutex_);
			if(is_closed_) {
				return status::closed;
			} else if (queue_->size() == capacity_) { // if full
				std::cout << "queue full, blocking..." << std::endl;
				// block on convar, do not proceed until either
				// queue is not full or the queue is closed
				convar_queue_not_full_.wait(lock, 
						[this] { return (queue_->size() < capacity_) ||
										 is_closed_;});
				if(is_closed_) { 
					std::cout << "queue closed, push returning status::closed" << std::endl;
					return status::closed; 
				} 
				std::cout << "[push]  Unblocking, ";
				queue_->push(x);
//				std::cout << "pushed " << x << " to queue" << std::endl;
				convar_queue_not_empty_.notify_one();
				return status::success;
			} else {
				queue_->push(x);
				convar_queue_not_empty_.notify_one();
//				std::cout << "pushed " << x << " to queue" << std::endl;
				return status::success;
			}
		}

		// Removes the value from the front of the queue and places it
		// in x, blocking if necessary.
		// If the queue is empty and not closed, the thread is blocked
		// until: 1) a value can be removed from the queue; or 2) the
		// queue is closed.
		// If the queue is closed, the function does not block and either
		// returns status::closed or status::success, depending on whether
		// a value can be successfully removed from the queue.
		// If a value is successfully removed from the queue, the value
		// is placed in x and the function returns status::success.
		// If a value cannot be successfully removed from the queue (due to
		// the queue being both empty and closed), the function returns
		// status::closed.
		// This function is thread safe.
		status pop(value_type& x)
		{
			std::unique_lock lock(queue_mutex_);
			if(!queue_->empty()) { // if not empty
				// Note: whether closed or not, same behaviour
				// is desired for a non-empty queue.
				// Set x = oldest element, 
				// signal convar to indicate queue is not empty,
				// return success
				x = std::move(queue_->front());
				queue_->pop();
				convar_queue_not_full_.notify_one();
				return status::success;
			} else if(is_closed()) { // it is closed and empty
				std::cout << "queue closed and empty, pop returning status::closed" << std::endl;
				return status::closed;
			} else { // it is not closed, but it is empty
				// block thread.  If queue becomes not full
				// or blocked, wake up thread
				convar_queue_not_empty_.wait(lock,
						[this] {return (!queue_->empty() ||	
										is_closed());});
				if(is_closed_) { return status::closed; }
				x = std::move(queue_->front());
				queue_->pop();
				convar_queue_not_full_.notify_one();
				return status::success;
			}
		}

		// Closes the queue.
		// The queue is placed in the closed state.
		// The closed state prevents more items from being inserted
		// on the queue, but it does not clear the items that are
		// already on the queue.
		// Invoking this function on a closed queue has no effect.
		// This function is thread safe.
		void close()
		{
			// lock mutex
			std::scoped_lock lock(queue_mutex_);
			// set queue to a locked state
			is_closed_ = true;
			// wake up all threads currently blocked 
			// waiting to push to queue (no more items
			// are to be pushed on closed queue)
			convar_queue_not_full_.notify_all();
			convar_queue_not_empty_.notify_all();
		}

		// Clears the queue.
		// All of the elements on the queue are discarded.
		// This function is thread safe.
		void clear() {
			std::cout << "clear called" << std::endl;
			// lock mutex
			std::scoped_lock lock(queue_mutex_);
			while(!is_empty()) {
				T temp;
				pop(temp);
			}
		}

		// Returns if the queue is currently full (i.e., the number of
		// elements in the queue equals the maximum queue size).
		// This function is not thread safe.
		bool is_full() const { return queue_->size() == capacity_; }

		// Returns if the queue is currently empty.
		// This function is not thread safe.
		bool is_empty() const {	return queue_->empty(); }

		// Returns if the queue is closed (i.e., in the closed state).
		// This function is not thread safe.
		bool is_closed() const { return is_closed_;	}

		// Returns the maximum number of elements that can be held in
		// the queue.
		// This function is not thread safe.
		size_type max_size() const { return capacity_; }

	private:
		// pointer to queue of value_type objects, underlying ADT is std::list
		std::queue<value_type, std::list<value_type> >* queue_;
		size_type capacity_; // maximum capacity of queue_
		mutable std::mutex queue_mutex_;
		mutable std::condition_variable convar_queue_not_full_;
		mutable std::condition_variable convar_queue_not_empty_;
		bool is_closed_;
	}; // queue class
}} // namespace
#endif
