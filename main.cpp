#include "enumerate.hpp"

#include <vector>
#include <iostream>

struct A {
    A() = default;
    A(A const &) {
        std::cout << "A copy" << std::endl;
    }
    A(A &&) noexcept {
        std::cout << "A move" << std::endl;
    }
};

int main() {
    std::vector<int> v = {5, 6, 7, 10};

    for (auto const [i, x] : enumerate(v)) {
        std::cout << "v[" << i << "] = " << x << std::endl;
    }
    /*
        v[0] = 5
        v[1] = 6
        v[2] = 7
        v[3] = 10
     */

    for (auto [i, x] : enumerate(v)) {
        x = static_cast<int>(i * i + 4);
        // i = 4; compilation error
    }

    std::vector<int> t = {2, 2, 6};
    for (auto [i, x] : enumerate(std::move(t))) {

    }

    for (auto [i, x] : enumerate(v)) {
        std::cout << "v[" << i << "] = " << x << std::endl;
    }
    /*  v[0] = 4
        v[1] = 5
        v[2] = 8
        v[3] = 13
     */

    const std::vector<int> v2 = {1,2,3,4};
    for (auto [i, x] : enumerate(v2)) {
        // x = 5;  compilation error
    }

    for (auto [i, x] : enumerate("easy")) {
        // x = 'x'; compilation error
        std::cout << i << ' ' << x << std::endl;
    }
    /*
        0 e
        1 a
        2 s
        3 y
        4 \0
     */

    int arr[] = {2, 4, 5};
    for (auto [i, x] : enumerate(arr)) {
        x += static_cast<int>(i);
    }
    std::cout << arr[0] << ' '
              << arr[1] << ' '
              << arr[2] << std::endl;
    // 2 5 7


    std::vector<A> as = {{}, {}, {}};
    std::cout << "JERE\n";
    for (auto [i, x] : enumerate(as)) {}

    for (auto [i, x] : enumerate({1, 2, 3})) {
        std::cout << x << ' ' ;
    }
    std::cout << std::endl;


    return 0;
}