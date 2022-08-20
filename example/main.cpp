#include <zip_utils/zip_utils.hpp>

#include <string_view>
#include <iostream>

using namespace zip_utils::views;
using std::operator""sv;

int main() {
    for (auto [idx, chr] : enumerate("Hello, world!"sv)) {
        std::cout << "string [" << idx << "] = " << chr << std::endl;
    }
    return 0;
}