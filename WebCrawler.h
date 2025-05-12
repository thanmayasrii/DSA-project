#ifndef WEBCRAWLER_H
#define WEBCRAWLER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

class WebCrawler {
public:
    void Crawl(const std::string& startUrl, int maxDepth);


private:
    std::unordered_set<std::string> discoveredWebsites;
    std::unordered_map<std::string, std::vector<std::string> > adjacencyList;
    std::unordered_map<std::string, double> pageRank;

    std::string ReadRawHtml(const std::string& url);
    std::vector<std::string> GetLinksFromHtml(const std::string& rawHtml);
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp);
    void ComputePageRank();
    void DisplayPageRanks();
};

#endif
