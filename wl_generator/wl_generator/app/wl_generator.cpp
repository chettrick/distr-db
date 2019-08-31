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

	CURL *curl;
	CURLcode res;
	curl = curl_easy_init();
	if(curl) {
		struct curl_slist *slist1 = NULL;
		slist1 = curl_slist_append(slist1, "Content-Type: application/json");
		curl_easy_setopt(curl, CURLOPT_URL, "http://trans-srv:5555/api/movies");
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "{\"name\":\"The movie about workload generators\",\"date\":\"2019\"}");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist1);
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");

		// Now actually perform the http request
		res = curl_easy_perform(curl);
		res = curl_easy_perform(curl);

		curl_easy_cleanup(curl);
	}

	// making use of JSON, http post request to the new transaction server
	/*
	CURLcode ret;
	CURL *hnd;
	struct curl_slist *slist1;
	std::string jsonstr = "{\"name\":\"movie name\":\"date\":\"2019\"}";

	slist1 = NULL;
	slist1 = curl_slist_append(slist1, "Content-Type: application/json");
	std::string url_str = "http://trans-srv:5555/api/movies";

	hnd = curl_easy_init();
	curl_easy_setopt(hnd, CURLOPT_URL, url_str.c_str());
	curl_easy_setopt(hnd, CURLOPT_NOPROGRESS, 1L);
	curl_easy_setopt(hnd, CURLOPT_POSTFIELDS, jsonstr.c_str());
	curl_easy_setopt(hnd, CURLOPT_USERAGENT, "curl/7.38.0");
	curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, slist1);
	curl_easy_setopt(hnd, CURLOPT_MAXREDIRS, 50L);
	curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "POST");
	curl_easy_setopt(hnd, CURLOPT_TCP_KEEPALIVE, 1L);

	ret = curl_easy_perform(hnd);

	curl_easy_cleanup(hnd);
	hnd = NULL;
	curl_slist_free_all(slist1);
	slist1 = NULL;

	********************************************JSON ATTEMPT ABOVE*******/


	// The commented out code is an attempt at making use of the SFML library
	// to make http post requests.
	/*
	sf::Http http("http://transaction-server-quart:5000");
	sf::Http::Request request;
	request.setMethod(sf::Http::Request::Post);
	request.setUri("/");
	request.setHttpVersion(1,1);
	request.setField("From", "me");
	request.setField("To", "you");
	request.setBody("para1=value1&para2=value2");

	sf::Http::Response response = http.sendRequest(request);
	*/
	/*
	std::string name = "random name";
	int score = 15;

		// prepare the request
		sf::Http::Request request("/send-score.php", sf::Http::Request::Post);

		// encode the parameters in the request body
		std::ostringstream stream;
		stream << "name=" << name << "&score=" << score;
		request.setBody(stream.str());

		// send the request
		sf::Http http("http://www.myserver.com/");
		sf::Http::Response response = http.sendRequest(request);

		 */
	using function_type = std::function<void()>;
	const std::size_t QUEUE_SIZE = 500;
	ra::concurrency::thread_pool tp;
	tp.schedule(std::move(fun));
	std::cout << "finished tester, queue and threadpool made" << std::endl;
	return 0;
}
