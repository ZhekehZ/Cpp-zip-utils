#include <zip_utils/zip_utils.hpp>

#include <catch.hpp>

#include <array>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string_view>
#include <vector>

using namespace zip_utils;
using namespace std::literals;

TEST_CASE("References", "[zip]") {
    static bool copied = false;
    static bool moved = false;
    bool value_changed = false;

    struct A {
        A() = default;
        A(const A&) {
            copied = true;
        }
        A(A&&)
        noexcept {
            moved = true;
        }
        A& operator=(const A&) {
            copied = true;
            return *this;
        }
        A& operator=(A&&) noexcept {
            moved = true;
            return *this;
        }

        int value = 0;
    };

    A a[] = {{}};
    int b[] = {0};

    {
        copied = false;
        moved = false;
        a[0].value = 0;
        for (auto [x, y] : zip(a, b)) {
            x.value = 3;
        }
        value_changed = a[0].value != 0;
        REQUIRE((copied && !moved && !value_changed));
    }
    {
        copied = false;
        moved = false;
        a[0].value = 0;
        for (auto& [x, y] : zip(a, b)) {
            x.value = 3;
        }
        value_changed = a[0].value != 0;
        REQUIRE((!copied && !moved && value_changed));
    }
    {
        copied = false;
        moved = false;
        a[0].value = 0;
        for (auto&& [x, y] : zip(a, b)) {
            x.value = 3;
        }
        value_changed = a[0].value != 0;
        REQUIRE((!copied && !moved && value_changed));
    }
}

TEST_CASE("Modification", "[zip]") {
    std::vector u = {1, 2, 3, 4, 5};
    std::vector v = {'x', 'y', 'z'};

    for (auto [x, y] : zip(u, v)) {
        x = y;
    }
    REQUIRE((v[0] != u[0] && v[1] != u[1] && v[2] != u[2]));

    for (auto& [x, y] : zip(u, v)) {
        x = y;
    }
    REQUIRE((v[0] == u[0] && v[1] == u[1] && v[2] == u[2]));
}

TEST_CASE("Containers", "[zip]") {
    std::vector v = {1, 2, 3, 0, 0, 0};
    std::set s = {2, 3, 4};
    std::map<int, std::string> m = {{2, "x"}, {4, "y"}, {8, "no"}};

    for (auto [x, y, z] : zip(v, s, m)) {
        REQUIRE((x + 1 == y && static_cast<unsigned>(z.first) == 1u << static_cast<unsigned>(x)));
    }
}

TEST_CASE("Built-in arrays", "[zip]") {
    int A[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 1000};
    char B[] = {5, 4, 3, 2, 1};

    for (auto& [a, b] : zip(A, B)) {
        REQUIRE(a == 6 - b);
    }
}

TEST_CASE("C-string", "[zip]") {
    std::stringstream ss;

    auto message1 = "hello, world! ;)"sv;
    auto message2 = "...."sv;
    for (auto [x, y] : zip(message1, message2)) {
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

    for (auto [x] : zip(std::as_const(v))) {
        STATIC_REQUIRE(std::is_const_v<decltype(x)>);
    }

    for (auto& [x] : zip(v)) {
        STATIC_REQUIRE(!std::is_const_v<decltype(x)>);
    }

    for (auto const& [x] : zip(v)) {
        STATIC_REQUIRE(std::is_const_v<decltype(x)>);
    }

    for (auto& [x] : zip(std::as_const(v))) {
        STATIC_REQUIRE(std::is_const_v<decltype(x)>);
    }

    for (auto const& [x] : zip(std::as_const(v))) {
        STATIC_REQUIRE(std::is_const_v<decltype(x)>);
    }
}

TEST_CASE("Initializer list", "[enumerate]") {
    auto numbers = {1u, 2u, 3u, 4u, 5u};
    for (auto [i, x] : enumerate(numbers)) {
        REQUIRE(i + 1 == x);
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
        {0, "e"}};
    std::stringstream ss;

    for (auto [i, x] : enumerate(m)) {
        ss << i << ' ' << x.first << ',' << x.second << ' ';
    }

    REQUIRE(ss.str() == "0 0,e 1 2,a 2 44,c 3 52,d 4 324,b ");
}

TEST_CASE("Built-in", "[enumerate]") {
    std::vector<int> simple_array[] = {{2}, {4}, {8}, {16}};

    for (auto [i, x] : enumerate(simple_array)) {
        REQUIRE(static_cast<unsigned>(x[0]) == 1u << (i + 1));
    }
}

TEST_CASE("Enumerate stress", "[enumerate]") {
    std::vector<int> v;
    std::stringstream ss_enumerate, ss_expected;

    std::random_device rd;
    std::default_random_engine mt(rd());
    std::uniform_real_distribution<double> dist(1.0, 100.0);

    for (auto i = 0ul; i < 1e5l; ++i) {
        v.push_back(static_cast<int>(dist(mt)));
        ss_expected << "x[" << i << "]=" << v.back() << " ";
    }

    for (auto [i, x] : enumerate(v)) {
        ss_enumerate << "x[" << i << "]=" << x << " ";
    }

    REQUIRE(ss_enumerate.str() == ss_expected.str());
}

template<typename T, std::size_t N>
using arr_ref = T (&)[N];

template<std::size_t S, typename T, std::size_t N>
auto sub(arr_ref<T, N> a) -> arr_ref<T, N - S> {
    return reinterpret_cast<arr_ref<T, N - S>>(a[S]);
}

TEST_CASE("Example") {
    int F[10] = {0, 1};
    auto expected = {0, 1, 1, 2, 3, 5, 8, 13, 21, 34};
    using std::views::drop;

    for (auto& [x, y, z] : zip(F, F | drop(1), F | drop(2))) {
        z = x + y;
    }

    for (auto [i, x] : enumerate(expected)) {
        REQUIRE(F[i] == x);
    }
}

TEST_CASE("Constexpr") {
    constexpr auto sum = [](const auto& array) -> int {
        int result = 0;

        for (auto const& [x] : zip(array)) {
            result += x;
        }
        return result;
    };

    constexpr int array[] = {1, 2, 3, 4, 5};
    STATIC_REQUIRE(sum(array) == 15);

    constexpr std::array<int, 5> stl_array = {1, 43, 7, 3, 7};
    STATIC_REQUIRE(sum(stl_array) == 61);
}

TEST_CASE("Exceptions") {
    std::initializer_list<int> list = {1, 2, 3, 4, 5};
    std::vector<int> vec = {1, 2, 3, 4, 5};
    STATIC_REQUIRE(noexcept(zip(list)));
    STATIC_REQUIRE(!noexcept(zip(vec)));
}

TEST_CASE("Strong exception guarantee") {
    struct It {
        using value_type = int;
        using reference = int&;
        using iterator_category = std::forward_iterator_tag;
        using difference_type = int;

        It() = default;
        explicit It(int x)
        : i{x} {
        }
        It(It const&) = default;
        It& operator++() {
            if (++i == 5)
                throw 0;
            return *this;
        }
        It operator++(int) {
            It cp = *this;
            ++*this;
            return cp;
        }
        reference operator*() const {
            return i;
        }
        bool operator==(const It& o) const {
            return i == o.i;
        }
        bool operator!=(const It& o) const {
            return i != o.i;
        }
        mutable int i = 0;
    };

    struct S {
        static auto begin() {
            return It(0);
        }
        static auto end() {
            return It(10);
        }
    } s;

    int arr1[] = {3, 4, 5, 6, 7, 8, 9, 0, 1, 2};
    int arr2[] = {0, 8, 7, 6, 5, 4, 3, 2, 1, 0};
    auto z = zip(arr1, s, arr2);
    auto beg = z.begin();

    try {
        while (true) {
            ++beg;
        }
    }
    catch (int) {
    }

    auto [x, y, w] = *beg;
    REQUIRE(((x == 7) && (y == 4) && (w == 5)));
}

TEST_CASE("Skip") {
    std::array<int, 10> F = {0, 1};
    using std::views::drop;

    for (auto& [F0, F1, F2] : zip(F, F | drop(1), F | drop(2))) {
        F2 = F0 + F1;
    }

    auto expected = {0, 1, 1, 2, 3, 5, 8, 13, 21, 34};
    for (auto [i, x] : enumerate(expected)) {
        REQUIRE(F[i] == x);
    }

    std::vector v = {1, 2, 3, 4, 5};

    for (auto [x, y] : zip(v, v | drop(1))) {
        REQUIRE(x + 1 == y);
    }
}

TEST_CASE("Rvalue") {
    for (auto [i, x] : enumerate(std::vector<size_t>{0, 1, 2, 3, 4, 5, 6, 7, 8})) {
        REQUIRE(i == x);
    }
}

TEST_CASE("Rvalue-array") {
    size_t array[] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
    for (auto [i, x, y] : enumerate(std::vector<size_t>{0, 1, 2, 3, 4, 5, 6, 7, 8}, array)) {
        REQUIRE(i == x);
        REQUIRE(i == y);
    }

    size_t i = 0;
    for (auto x : zip(zip(array, array), zip(array, array))) {
        auto [ab, cd] = x;
        auto [a, b] = ab;
        auto [c, d] = cd;
        REQUIRE(i == a);
        REQUIRE(i == b);
        REQUIRE(i == c);
        REQUIRE(i == d);
        ++i;
    }
}

TEST_CASE("Rvalue no default constructor") {
    struct A : public std::vector<int> {
        A() = delete;
        explicit A(int) : std::vector<int>(0) {}
    };

    A arr(0);
    for (auto x : zip(arr)) {
        static_cast<void>(x);
    }
    for (auto x : zip(A(0))) {
        static_cast<void>(x);
    }
}