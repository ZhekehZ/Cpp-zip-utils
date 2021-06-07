#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <zip_utils/zip_utils.hpp>

#include <vector>
#include <array>
#include <sstream>
#include <set>
#include <map>
#include <stdexcept>

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
        auto zipped = zip(a, b);              \
        for (test [x, y] : zipped) {          \
            x.value = 3;                      \
        }                                     \
        value_changed = a[0].value != 0;      \
        REQUIRE((expects));                   \
    } while (0)

    REF_TEST_CASE(auto, copied && !moved && !value_changed);
    REF_TEST_CASE(auto &, !copied && !moved && value_changed);
    REF_TEST_CASE(auto &&, !copied && !moved && value_changed);
}

TEST_CASE("Modification", "[zip]") {
    std::vector v = {1, 2, 3, 4, 5};
    std::vector u = {'x', 'y', 'z'};

    auto zipped = zip(u, v);
    for (auto [x, y] : zipped) {
        x = y;
    }
    REQUIRE((v[0] != u[0] && v[1] != u[1] && v[2] != u[2]));

    for (auto & [x, y] : zipped) {
        x = y;
    }
    REQUIRE((v[0] == u[0] && v[1] == u[1] && v[2] == u[2]));
}

TEST_CASE("Containers", "[zip]") {
    std::vector v = {1, 2, 3, 0, 0, 0};
    std::set s = {2, 3, 4};
    std::map<int, std::string> m = {{2, "x"}, {4, "y"}, {8, "no"}};

    auto zipped = zip(v, s, m);
    for (auto [x, y, z] : zipped) {
        REQUIRE((x + 1 == y && static_cast<unsigned>(z.first) == 1u << static_cast<unsigned>(x)));
    }
}

TEST_CASE("Built-in arrays", "[zip]") {
    int a[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 1000};
    char b[] = {5, 4, 3, 2, 1};

    auto zipped = zip(a, b);
    for (auto & [a, b] : zipped) {
        REQUIRE(a == 6 - b);
    }
}

TEST_CASE("C-string", "[zip]") {
    std::stringstream ss;

    auto zipped = zip("hello, world! ;)"_sw, "...."_sw);
    for (auto [x, y] : zipped) {
        ss << x << y;
    }
    REQUIRE(ss.str() == "h.e.l.l.");
}

TEST_CASE("Zip as argument", "[zip]") {
    std::vector v = {1, 2, 3};
    std::vector u = {3, 6, 9};
    auto z = zip(u, v);

    std::stringstream ss;

    auto zipped = zip(z, z);
    for (auto [x, y] : zipped) {
        auto [a, b] = x;
        auto [c, d] = y;
        ss << a << ' ' << b << ' ' << c << ' ' << d << std::endl;
    }

    REQUIRE(ss.str() == "3 1 3 1\n6 2 6 2\n9 3 9 3\n");
}

TEST_CASE("Immutability", "[zip]") {
    std::vector<int> v;

    auto zipped = zip(v);
    for (auto [x] : zipped) {
        STATIC_REQUIRE(!std::is_const_v<decltype(x)>);
    }

    auto zipped_const = zip(std::as_const(v));
    for (auto [x] : zipped_const) {
        STATIC_REQUIRE(std::is_const_v<decltype(x)>);
    }

    for (auto & [x] : zipped) {
        STATIC_REQUIRE(!std::is_const_v<decltype(x)>);
    }

    for (auto const & [x] : zipped) {
        STATIC_REQUIRE(std::is_const_v<decltype(x)>);
    }

    for (auto & [x] : zipped_const) {
        STATIC_REQUIRE(std::is_const_v<decltype(x)>);
    }

    for (auto const & [x] : zipped_const) {
        STATIC_REQUIRE(std::is_const_v<decltype(x)>);
    }

}

TEST_CASE("Initializer list", "[enumerate]") {
    auto enumeration = enumerate({1, 2, 3, 4, 5});
    for (auto [i, x] : enumeration) {
        REQUIRE(static_cast<int>(i) + 1 == x);
    }
}

TEST_CASE("Vector", "[enumerate]") {
    std::vector v = {1, 2, 3, 4, 5};
    std::stringstream ss;

    auto enumeration = enumerate(v);
    for (auto [i, x] : enumeration) {
        ss << i << ' ' << x << ' ';
    }

    REQUIRE(ss.str() == "0 1 1 2 2 3 3 4 4 5 ");
}

TEST_CASE("Set", "[enumerate]") {
    std::set s = {'a', 'b', 'c', 'd', 'e'};
    std::stringstream ss;

    auto enumeration = enumerate(s);
    for (auto [i, x] : enumeration) {
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

    auto enumeration = enumerate(m);
    for (auto [i, x] : enumeration) {
        ss << i << ' ' << x.first << ',' << x.second << ' ';
    }

    REQUIRE(ss.str() == "0 0,e 1 2,a 2 44,c 3 52,d 4 324,b ");
}

TEST_CASE("Built-in", "[enumerate]") {
    std::vector<int> simple_array[] = {{2}, {4}, {8}, {16}};

    auto enumeration = enumerate(simple_array);
    for (auto [i, x] : enumeration) {
        REQUIRE(static_cast<unsigned>(x[0]) == 1u << (i + 1));
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

    auto enumeration = enumerate(v);
    for (auto [i, x] : enumeration) {
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

TEST_CASE("Example") {
    int F[10] = {0, 1};
    auto expected = enumerate({0, 1, 1, 2, 3, 5, 8, 13, 21, 34});

    auto fib_iter = zip(F, sub<1>(F), sub<2>(F));
    for (auto & [x, y, z] : fib_iter) {
        z = x + y;
    }

    for (auto [i, x] : expected) {
        REQUIRE(F[i] == x);
    }
}



TEST_CASE("Constexpr") {
    constexpr auto sum = [](const auto& array) -> int {
        int sum = 0;

        auto zipped = zip(array);
        for (auto const & [x] : zipped) {
            sum += x;
        }
        return sum;
    };

    constexpr int array[] = {1, 2, 3, 4, 5};
    STATIC_REQUIRE(sum(array) == 15);

    constexpr std::array<int, 5> stl_array = {1, 43, 7, 3, 7};
    STATIC_REQUIRE(sum(stl_array) == 61);
}

TEST_CASE("Exceptions") {
    std::initializer_list<int> list = {1, 2, 3, 4, 5};
    std::vector<int> vec = {1, 2, 3, 4, 5};
    STATIC_REQUIRE( noexcept(zip(list)) );
    STATIC_REQUIRE( !noexcept(zip(vec)) );
}

TEST_CASE("Strong exception guarantee") {
    struct It {
        using value_type = int;
        using reference = int &;
        using iterator_category = std::forward_iterator_tag;
        using difference_type = int;

        It() = default;
        explicit It(int x) : i{x} {}
        It(It const &) = default;
        It & operator++() { if (++i == 5) throw 0; return *this; }
        It operator++(int) { It cp = *this; ++*this; return cp; }
        reference operator*() const { return i; }
        bool operator==(const It & o) const { return i == o.i; }
        bool operator!=(const It & o) const { return i != o.i; }
        mutable int i = 0;
    };

    struct S {
        static auto begin() { return It(0); }
        static auto end() { return It(10); }
    };

    int arr1[] = {3, 4, 5, 6, 7, 8, 9, 0, 1, 2};
    int arr2[] = {0, 8, 7, 6, 5, 4, 3, 2, 1, 0};
    auto z = zip(arr1, S(), arr2);
    auto beg = z.begin();

    try {
        while (true) { ++beg; }
    } catch (int) { }

    auto [x, y, w] = *beg;
    REQUIRE(((x == 7) && (y == 4) && (w == 5)));
}

TEST_CASE("Skip") {
    std::array<int, 10> F = {0, 1};

    auto zipped = zip(F, F, F)
                    .skip<1>(1)
                    .skip<2>(2);
    for (auto & [F0, F1, F2] : zipped) {
        F2 = F0 + F1;
    }

    auto enumeration = enumerate({0, 1, 1, 2, 3, 5, 8, 13, 21, 34});
    for (auto [i, x] : enumeration) {
        REQUIRE(F[i] == x);
    }

    std::vector v = {1, 2, 3, 4, 5};

    auto zipped2 = zip(v, v).skip<1>(1);
    for (auto [x, y] : zipped2) {
        REQUIRE(x + 1 == y);
    }
}
