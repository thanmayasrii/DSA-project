#include "WebCrawler.h"
#include <queue>
#include <regex>
#include <iostream>
#include <curl/curl.h>
#include <cmath>
#include <fstream>
#include <sstream>
#include <algorithm>

using namespace std;

void WebCrawler::Crawl(const string& startUrl, int maxDepth) {
    queue<pair<string, int>> q;  
    q.push({startUrl, 0});  
    discoveredWebsites.insert(startUrl);

    while (!q.empty()) {
        string currentUrl = q.front().first;
        int currentDepth = q.front().second;
        q.pop();
        if (currentDepth >= maxDepth) {
            continue;
        }

        string html = ReadRawHtml(currentUrl);
        vector<string> links = GetLinksFromHtml(html);
        adjacencyList[currentUrl] = links;

        for (const auto& link : links) {
            if (discoveredWebsites.find(link) == discoveredWebsites.end()) {
                discoveredWebsites.insert(link);
                q.push({link, currentDepth + 1}); 
            }
        }
    }

    ComputePageRank();
    DisplayPageRanks();
}
vector<string> WebCrawler::GetLinksFromHtml(const string& rawHtml) {
    vector<string> links;
    regex urlRegex(R"(https?://[^\s\"'<>\)]+|file://[^\s\"'<>\)]+)");
    smatch match;
    auto start = rawHtml.cbegin();

    while (regex_search(start, rawHtml.cend(), match, urlRegex)) {
        links.push_back(match[0]);
        start = match.suffix().first;
    }

    return links;
}
size_t WebCrawler::WriteCallback(void* contents, size_t size, size_t nmemb, string* userp) {
    userp->append((char*)contents, size * nmemb);
    return size * nmemb;
}
string WebCrawler::ReadRawHtml(const string& url) {
    if (url.rfind("file://", 0) == 0) {
        string filePath = url.substr(7);
        ifstream file(filePath);
        if (!file.is_open()) {
            cerr << "Failed to open: " << filePath << endl;
            return "";
        }
        stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    } else if (url.rfind("http://", 0) == 0 || url.rfind("https://", 0) == 0) {
        CURL* curl = curl_easy_init();
        string content;
        if (curl) {
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &content);
            CURLcode res = curl_easy_perform(curl);
            if (res != CURLE_OK) {
                cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << endl;
            }
            curl_easy_cleanup(curl);
        }
        return content;
    }

    return "";
}


void WebCrawler::ComputePageRank() {
    const double damping = 0.85;
    const int maxIterations = 100;
    const double epsilon = 1.0e-6;

    for (const auto& [url, _] : adjacencyList) {
        pageRank[url] = 1.0;
    }

    for (int iter = 0; iter < maxIterations; ++iter) {
        unordered_map<string, double> newRank;
        for (const auto& [url, _] : pageRank) {
            newRank[url] = (1.0 - damping);
        }

        for (const auto& [from, tos] : adjacencyList) {
            if (tos.empty()) continue;
            double share = pageRank[from] / tos.size();
            for (const auto& to : tos) {
                newRank[to] += damping * share;
            }
        }

        double diff = 0.0;
        for (const auto& [url, val] : pageRank) {
            diff += abs(newRank[url] - val);
        }

        pageRank = newRank;
        if (diff < epsilon) break;
    }
}
void WebCrawler::DisplayPageRanks() {
    vector<pair<string, double>> rankedPages(pageRank.begin(), pageRank.end());

    sort(rankedPages.begin(), rankedPages.end(),
         [](const auto& a, const auto& b) { return a.second > b.second; });
    for (const auto& [url, rank] : rankedPages) {
        cout << url << " : " << rank << "\n";
    }
}
