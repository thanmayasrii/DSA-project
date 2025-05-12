#include "WebCrawler.h"
#include <iostream>
using namespace std;

int main() {
    WebCrawler crawler;
    string startUrl;

    cout << "Enter start URL (e.g., https://www.example.com or file:///path/to/a.html): ";
    cin >> startUrl;
    int depth;
    cout<<"Enter depth : ";cin>>depth;
    crawler.Crawl(startUrl,depth);

    return 0;
}
