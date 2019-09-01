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

// #include <SFML/Network.hpp>
//#include <sstream>

using namespace std;
     
int main()
{
	std::cout << "added libcurl" << std::endl;
	std::function<void()> fun = []() { std::cout << "running lambda function" << std::endl; };
	std::thread t1(fun);
	t1.join();

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

	using function_type = std::function<void()>;
	const std::size_t QUEUE_SIZE = 500;
	ra::concurrency::thread_pool tp;
	tp.schedule(std::move(fun2));
	std::cout << "finished tester, queue and threadpool made" << std::endl;
	return 0;
}
