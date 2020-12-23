#include<string>
#include<iostream>
#include<optional>
#include<vector>
#include<utility>
#include<regex>

#include<curl/curl.h>
#include"libfort_single_header.hpp"

const std::string USAGE = "Usage:\n dictcc <WORD_TO_SEARCH_FOR>";
const std::string BASE_STRING = "https://www.dict.cc/?s=";
const std::regex EXTRACT_REGEX("\"(.*?)\",?");

static size_t WriteCallback(void *contents,
			    size_t size,
			    size_t nmemb,
			    void *userp) {
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

static std::optional<std::string> DownloadHTML(std::string searchTerm) {
	CURL *curl;
	CURLcode res;
	std::string readBuffer;

	curl = curl_easy_init();
	if (!curl) {
		std::cerr << "Failed to init curl!" << std::endl;
		return {};
	}

	curl_easy_setopt(curl, CURLOPT_URL, (BASE_STRING + searchTerm).c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

	res = curl_easy_perform(curl);
	if (res != CURLE_OK) {
		std::cerr << "HTML Download failed with the following Error: "
			<< curl_easy_strerror(res);
		return {};
	}
	return readBuffer;
}

static std::vector<std::string> extractLanguage(const std::string l) {
	std::vector<std::string> ret;
	for (auto i = std::sregex_iterator(l.begin(), l.end(), EXTRACT_REGEX);
	     i != std::sregex_iterator();
	     ++i) {
		std::string s = (*i).str();
		s = s.substr(s.find('"', 0)+1, s.rfind('"', s.length())-1);
		if (s.length()) {
			ret.push_back(s);
		}
	}
	return ret;
}

static std::vector<std::pair<std::string, std::string>> extractPairs(
								     const std::string html) {
	std::istringstream stream(html);

	// We look for the following lines
	// var c{1,2}Arr = new Array("","...);
	// we know that c2Arr follows c1Arr
	std::string l1, l2;
	for (std::getline(stream, l1);
	     l1.rfind("var c1Arr", 0) != 0;
	     std::getline(stream, l1));
	std::getline(stream, l2);

	std::vector<std::string> v1 = extractLanguage(l1),
		v2 = extractLanguage(l2);

	std::vector<std::pair<std::string, std::string>> res;
	for (size_t i=0; i<v1.size(); ++i) {
		const auto temp = std::make_pair(v1[i], v2[i]);
		res.push_back(temp);
	}
	return res;
}


int main(int argc, char **argv) {
	if (argc == 1) {
		std::cerr << USAGE << std::endl;
		return 1;
	}
	auto const htmlOptional = DownloadHTML(argv[1]);
	if (!htmlOptional.has_value()) {
		return 1;
	}
	auto const html = htmlOptional.value();
	auto const pairs = extractPairs(html);
	fort::char_table table;
	table << fort::header << "English" << "Deutsch" << fort::endr;
	for (auto p : pairs) {
		table << p.first << p.second << fort::endr;
	}
	std::cout << table.to_string() << std::endl;
	return 0;
}
