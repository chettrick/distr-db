// wl_generator.cpp
// Used to mimick front end of CSC462 front-end when 
// stress testing the system

#include <iostream>
#include <string>
#include <functional>
#include <thread>
#include "ra/queue.hpp"
#include "ra/thread_pool.hpp"
#include <curl/curl.h>


using namespace std;
     
// Factory function
// This returns a lambda function that inserts a record in the database system
auto make_post_function(std::string name, std::string descr, std::string date) {
	return [name, descr, date]()
	{
		CURL *curl;
		CURLcode res;
		curl = curl_easy_init();
		std::string post_data = "{\"name\":\"" + name + "\",\"date\":\"" + date + "\"}";
		std::cout << "post data: " << post_data << std::endl;

		if(curl) {
			struct curl_slist *slist1 = NULL;
			slist1 = curl_slist_append(slist1, "Content-Type: application/json");
			curl_easy_setopt(curl, CURLOPT_URL, "http://trans-srv:5555/api/movies");
			curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);

			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data.c_str());
			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist1);
			curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");

			// Now actually perform the http request
			res = curl_easy_perform(curl);
			curl_easy_cleanup(curl);
		}
	};
}

// create a series of http POST requests, these will cause records to be written
// to the movies table.
// num_records determines how many records are to be added
void add_records(	ra::concurrency::thread_pool &tp,
			const unsigned long long num_records,
			const std::string init_name,
			const std::string init_descr,
			const std::string init_date) {
	for(long long i = 0; i < num_records; ++i) {
		std::cout << "current movie name: " << (init_name + std::to_string(i)) << std::endl;

		// Create args for the factory function,
		// these will be the values in this specific record
		auto suffix = std::to_string(i);
		auto cur_name = init_name + suffix,
		     cur_descr = init_descr + suffix,
		     cur_date = init_date + suffix;
		auto cur_fun = make_post_function(cur_name, cur_descr, cur_date);
		tp.schedule(cur_fun);
	}
}

int main()
{
	std::function<void()> fun2 = []() { 
		std::cout << "running http request" << std::endl; 
			CURL *curl;
			CURLcode res;
			curl = curl_easy_init();
			if(curl) {
				struct curl_slist *slist1 = NULL;
				slist1 = curl_slist_append(slist1, "Content-Type: application/json");
				curl_easy_setopt(curl, CURLOPT_URL, "http://trans-srv:5555/api/movies");
				curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
				curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "{\"name\":\"The bigger better movie about workload generators\",\"date\":\"2020\"}");
				curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist1);
				curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");

				// Now actually perform the http request
				res = curl_easy_perform(curl);
				res = curl_easy_perform(curl);

				curl_easy_cleanup(curl);
			}
	}; // end of lambda function fun2

	// No idea why, but the compiler complains unless I create and join on a thread
	// so this code (3 lines) below is here just to keep gcc happy...  
	std::function<void()> fun = []() {std::cout << "running wl_generator" << std::endl;};
	std::thread t1(std::move(fun));
	t1.join();


	// Testing factory function, just ensuring the strings are being modified
	// as expected

	using function_type = std::function<void()>;
	const std::size_t QUEUE_SIZE = 500;
	ra::concurrency::thread_pool tp(100);
	add_records(tp, 5, "movie_name", "movie_description", "2019");
//	tp.schedule(std::move(fun2));
	std::cout << "finished tester, queue and threadpool made" << std::endl;
	return 0;
}
