// wl_generator.cpp
// Used to mimick front end of CSC462 front-end when 
// stress testing the system

#include <iostream>
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
		curl_easy_setopt(curl, CURLOPT_URL, "http://transaction-server-quart:5000/something");
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "name=daniel&project=curl");
		res = curl_easy_perform(curl);

		curl_easy_cleanup(curl);
	}


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
