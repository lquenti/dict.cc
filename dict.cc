#include<cstdio>
#include<curl/curl.h>

#include<string>
#include<iostream>

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}


int main(int argc, char **argv) {
	CURL *curl;
	CURLcode res;
	std::string readBuffer;

	curl = curl_easy_init();

	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, "https://www.dict.cc/?s=test");

		// Set callback
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);


		// Perform request
		res = curl_easy_perform(curl);
		if (res != CURLE_OK) {
			fprintf(stderr, "curl_easy_perform failed(): %s\n",
				curl_easy_strerror(res));
		}
		// cleanup
		curl_easy_cleanup(curl);

		std::cout << readBuffer << std::endl;

	}
	return 0;
}