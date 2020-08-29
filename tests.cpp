#define CATCH_CONFIG_MAIN
#include "catch/catch.hpp"
#include "zip_utils.hpp"

#include <vector>
#include <sstream>
#include <set>
#include <map>

using namespace zip_utils;

TEST_CASE("References", "[zip]") {
    static bool copied = false;
    static bool moved = false;
    bool value_changed = false;

    struct A {
        A() = default;
        A(const A &) {
            copied = true;
        }
        A(A &&) noexcept {
            moved = true;
        }
        A & operator=(const A &) {
            copied = true;
            return *this;
        }
        A & operator=(A &&) noexcept {
            moved = true;
            return *this;
        }

        int value = 0;
    };

    A a[] = {{}};
    int b[] = {0};

    #define REF_TEST_CASE(test, expects) do { \
        copied = false;                       \
        moved = false;                        \
        a[0].value = 0;                       \
        for (test [x, y] : zip(a, b)) {       \
            x.value = 3;                      \
        }                                     \
        value_changed = a[0].value != 0;      \
        REQUIRE((expects));                   \
    } while (0, 0)

    REF_TEST_CASE(auto, copied && !moved && !value_changed);
    REF_TEST_CASE(auto &, !copied && !moved && value_changed);
    REF_TEST_CASE(auto &&, !copied && !moved && value_changed);
}

TEST_CASE("Modification", "[zip]") {
    std::vector v = {1, 2, 3, 4, 5};
    std::vector u = {'x', 'y', 'z'};

    for (auto [x, y] : zip(u, v)) {
        x = y;
    }
    REQUIRE((v[0] != u[0] && v[1] != u[1] && v[2] != u[2]));


    for (auto & [x, y] : zip(u, v)) {
        x = y;
    }
    REQUIRE((v[0] == u[0] && v[1] == u[1] && v[2] == u[2]));
}

TEST_CASE("Containers", "[zip]") {
    std::vector v = {1, 2, 3, 0, 0, 0};
    std::set s = {2, 3, 4};
    std::map<int, std::string> m = {{2, "x"}, {4, "y"}, {8, "no"}};
    for (auto [x, y, z] : zip(v, s, m)) {
        REQUIRE((x + 1 == y && z.first == 1u << static_cast<unsigned>(x)));
    }
}

TEST_CASE("Built-in arrays", "[zip]") {
    int a[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 1000};
    char b[] = {5, 4, 3, 2, 1};
    for (auto & [a, b] : zip(a, b)) {
        REQUIRE(a == 6 - b);
    }
}

TEST_CASE("C-string", "[zip]") {
    std::stringstream ss;
    for (auto [x, y] : zip("hello, world! ;)"_sw, "...."_sw)) {
        ss << x << y;
    }
    REQUIRE(ss.str() == "h.e.l.l.");
}

TEST_CASE("Zip as argument", "[zip]") {
    std::vector v = {1, 2, 3};
    std::vector u = {3, 6, 9};
    auto z = zip(u, v);

    std::stringstream ss;

    for (auto [x, y] : zip(z, z)) {
        auto [a, b] = x;
        auto [c, d] = y;
        ss << a << ' ' << b << ' ' << c << ' ' << d << std::endl;
    }

    REQUIRE(ss.str() == "3 1 3 1\n6 2 6 2\n9 3 9 3\n");
}

TEST_CASE("Immutability", "[zip]") {
    std::vector<int> v;

    for (auto [x] : zip(v)) {
        STATIC_REQUIRE(!std::is_const_v<decltype(x)>);
    }

    for (auto const [x] : zip(v)) {
        STATIC_REQUIRE(std::is_const_v<decltype(x)>);
    }

    for (auto [x] : zip(std::as_const(v))) {
        STATIC_REQUIRE(std::is_const_v<decltype(x)>);
    }

    for (auto const [x] : zip(std::as_const(v))) {
        STATIC_REQUIRE(std::is_const_v<decltype(x)>);
    }

    for (auto & [x] : zip(v)) {
        STATIC_REQUIRE(!std::is_const_v<decltype(x)>);
    }

    for (auto const & [x] : zip(v)) {
        STATIC_REQUIRE(std::is_const_v<decltype(x)>);
    }

    for (auto & [x] : zip(std::as_const(v))) {
        STATIC_REQUIRE(std::is_const_v<decltype(x)>);
    }

    for (auto const & [x] : zip(std::as_const(v))) {
        STATIC_REQUIRE(std::is_const_v<decltype(x)>);
    }

}

TEST_CASE("Initializer list", "[enumerate]") {
    for (auto [i, x] : enumerate({1, 2, 3, 4, 5})) {
        REQUIRE(i + 1 == x);
    }
}

TEST_CASE("Vector", "[enumerate]") {
    std::vector v = {1, 2, 3, 4, 5};
    std::stringstream ss;

    for (auto [i, x] : enumerate(v)) {
        ss << i << ' ' << x << ' ';
    }

    REQUIRE(ss.str() == "0 1 1 2 2 3 3 4 4 5 ");
}

TEST_CASE("Set", "[enumerate]") {
    std::set s = {"a", "b", "c", "d", "e"};
    std::stringstream ss;

    for (auto [i, x] : enumerate(s)) {
        ss << i << ' ' << x << ' ';
    }

    REQUIRE(ss.str() == "0 a 1 b 2 c 3 d 4 e ");
}

TEST_CASE("Map", "[enumerate]") {
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

TEST_CASE("Built-in", "[enumerate]") {
    std::vector<int> simple_array[] = {{2}, {4}, {8}, {16}};
    for (auto [i, x] : enumerate(simple_array)) {
        REQUIRE(x[0] == 1 << i + 1);
    }
}

TEST_CASE("Enumerate stress <kek>", "[enumerate]") {
    std::vector<int> v;
    std::stringstream ss_enumerate, ss_expected;

    std::random_device rd;
    std::default_random_engine mt(rd());
    std::uniform_real_distribution<double> dist(1.0, 100.0);

    for (auto i = 0ul; i < 1e5; ++i) {
        v.push_back(static_cast<int>(dist(mt)));
        ss_expected << "x[" << i << "]=" << v.back() << " ";
    }

    for (auto [i, x] : enumerate(v)) {
        ss_enumerate << "x[" << i << "]=" << x << " ";
    }

    REQUIRE(ss_enumerate.str() == ss_expected.str());
}

template <typename T, std::size_t N>
using arr_ref = T (&)[N];

template <std::size_t S, typename T, std::size_t N>
auto sub (arr_ref<T, N> a) -> arr_ref<T, N-S> {
    return reinterpret_cast<arr_ref<T, N-S>>(a[S]);
}


TEST_CASE("example") {
    int F[10] = {0, 1};

    for (auto & [x, y, z] : zip(F, sub<1>(F), sub<2>(F))) {
        z = x + y;
    }

    for (auto [i, x] : enumerate({0, 1, 1, 2, 3, 5, 8, 13, 21, 34})) {
        REQUIRE(F[i] == x);
    }
}
