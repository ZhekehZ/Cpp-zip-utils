#define CATCH_CONFIG_MAIN
#include "catch/catch.hpp"
#include "enumerate.hpp"

#include <vector>
#include <sstream>
#include <set>
#include <map>

TEST_CASE("Vector", "[Core]") {
    std::vector v = {1, 2, 3, 4, 5};
    std::stringstream ss;

    for (auto [i, x] : enumerate(v)) {
        ss << i << ' ' << x << ' ';
    }

    REQUIRE(ss.str() == "0 1 1 2 2 3 3 4 4 5 ");
}

TEST_CASE("Set", "[Core]") {
    std::set s = {"a", "b", "c", "d", "e"};
    std::stringstream ss;

    for (auto [i, x] : enumerate(s)) {
        ss << i << ' ' << x << ' ';
    }

    REQUIRE(ss.str() == "0 a 1 b 2 c 3 d 4 e ");
}

TEST_CASE("Map", "[Core]") {
    std::map<int, const char*> m = {
            {2, "a"},
            {324, "b"},
            {44, "c"},
            {52, "d"},
            {0, "e"}
    };
    std::stringstream ss;

    for (auto [i, x] : enumerate(m)) {
        ss << i << ' ' << x.first << ',' << x.second << ' ';
    }

    REQUIRE(ss.str() == "0 0,e 1 2,a 2 44,c 3 52,d 4 324,b ");
}

TEST_CASE("Built-in", "[Core]") {
    std::vector<int> simple_array[] = {{2}, {4}, {8}, {16}};
    for (auto [i, x] : enumerate(simple_array)) {
        REQUIRE(x[0] == 1 << i + 1);
    }
}

TEST_CASE("Initializer_list", "[Core]") {
    for (auto [i, x] : enumerate({3, 2, 1})) {
        REQUIRE(x == 3 - i);
    }
}

TEST_CASE("C-string", "[Core]") {
    for (auto [i, x] : enumerate("abcdefghijklmnopqrstuvwxyz")) {
        REQUIRE(x == 'a' + i);
    }
}

TEST_CASE("Modification", "[Const]") {
    std::vector v = {0ul, 0ul, 0ul, 0ul, 0ul};

    for (auto [i, x] : enumerate(v)) {
        x = i;
    }

    for (auto i = 0ul; i < v.size(); ++i) {
        REQUIRE(v[i] == i);
    }
}

TEST_CASE("Immutability", "[Const]") {
    std::vector<int> v;

    for (auto [i, x] : enumerate(v)) {
        STATIC_REQUIRE(std::is_assignable_v<decltype(x), int>);
    }

    for (auto [i, x] : enumerate(std::as_const(v))) {
        STATIC_REQUIRE(!std::is_assignable_v<decltype(x), int>);
    }
}

TEST_CASE("Basic stress", "[Stress]") {
    std::vector<int> v;
    std::stringstream ss_enumerate, ss_expected;

    for (auto i = 0ul; i < 1e5; ++i) {
        v.push_back(rand());
        ss_expected << "x[" << i << "]=" << v.back() << " ";
    }

    for (auto [i, x] : enumerate(v)) {
        ss_enumerate << "x[" << i << "]=" << x << " ";
    }

    REQUIRE(ss_enumerate.str() == ss_expected.str());
}

TEST_CASE("No copy no move") {
    static bool passed = true;

    struct A {
        A() = default;
        A(A const &) { passed = false; }
        A(A &&) noexcept { passed = false; }
        A& operator=(A const &) { passed = false; }
        A& operator=(A &&) noexcept { passed = false; }
    };

    std::vector<A> v(10);
    for (auto [i, x] : enumerate(v)) {
        REQUIRE(passed);
    }
}
