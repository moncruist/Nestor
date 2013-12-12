#include <iostream>
#include <string>
#include <sstream>
#include "net/http_client.h"

using namespace std;
using namespace nestor::net;

int main(int argc, char *argv[]) {
    ostringstream oss;
    HttpClient client("en.wikipedia.org");
    auto page = client.getResource("/wiki/Main_Page");
    for (auto c : *page) {
        oss << c;
    }
    cout << oss.str();
}
