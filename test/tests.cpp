#include <zip_utils/zip_utils.hpp>

#include <catch2/catch.hpp>

#include <array>
#include <map>
#include <ranges>
#include <set>
#include <sstream>
#include <string_view>
#include <vector>

using namespace zip_utils::views;
using namespace std::literals;

template<int* copy_count, int* move_count>
struct CopyMoveCounter {
    explicit CopyMoveCounter(int value)
        : value(value)
    {};

    CopyMoveCounter(const CopyMoveCounter& other) noexcept {
        value = other.value;
        ++*copy_count;
    }
    CopyMoveCounter(CopyMoveCounter&& other) noexcept {
        std::swap(value, other.value);
        ++*move_count;
    }
    CopyMoveCounter& operator=(const CopyMoveCounter& other) noexcept {
        value = other.value;
        ++*copy_count;
        return *this;
    }
    CopyMoveCounter& operator=(CopyMoveCounter&& other) noexcept {
        std::swap(value, other.value);
        ++*move_count;
        return *this;
    }
    static void reset() {
        *copy_count = 0;
        *move_count = 0;
    };
    static int copies() {
        return *copy_count;
    }
    static int moves() {
        return *move_count;
    }

    int value = 0;
};

#define NewTypeCMC decltype([] {      \
static int copy_count = 0;            \
static int move_count = 0;            \
return CopyMoveCounter<&copy_count, &move_count>(0); \
}())


TEST_CASE("References", "[zip]") {
    using zip_utils::configuration::zip_config;

    SECTION("auto [x, y] = zip(a, b)"){
        using A = NewTypeCMC;

        std::array<A, 1> a = { A(0) };
        std::array<int, 1> b{};

        INFO("`a` is passed by reference, and its elements are copied."
             "Assignment doesn't affect `a`.");
        for (auto [x, y] : zip(a, b)) {
            x.value = 3;
        }

        REQUIRE(A::copies() == 1);
        REQUIRE(A::moves() == 0);
        REQUIRE(a[0].value == 0);
    }


    SECTION("auto & [x, y] = zip(a, b)"){
        using A = NewTypeCMC;

        std::array<A, 1> a = { A(0) };
        std::array<int, 1> b{};

        INFO("`a` and its elements are passed by reference."
             "Assignment changes `a`'s elements.");
        for (auto& [x, y] : zip(a, b)) {
            x.value = 3;
        }

        REQUIRE(A::copies() == 0);
        REQUIRE(A::moves() == 0);
        REQUIRE(a[0].value == 3);

        for (auto& [x, y] : zip(a, b)) {
            A _ = x;
            (void) _;
        }

        REQUIRE(A::copies() == 1);
        REQUIRE(A::moves() == 0);
    }

    SECTION("auto && [x, y] = zip(a, b)"){
        using A = NewTypeCMC;

        std::array<A, 1> a = { A(0) };
        std::array<int, 1> b{};

        INFO("`a` and its elements are passed by reference."
             "Assignment changes `a`'s elements.");
        for (auto&& [x, y] : zip(a, b)) {
            x.value = 3;
        }

        REQUIRE(A::copies() == 0);
        REQUIRE(A::moves() == 0);
        REQUIRE(a[0].value == 3);

        for (auto&& [x, y] : zip(a, b)) {
            A _ = x;
            (void) _;
        }

        REQUIRE(A::copies() == 1);
        REQUIRE(A::moves() == 0);
    }

    SECTION("auto [x, y] = zip(move(a), b)"){
        using A = NewTypeCMC;

        std::array<A, 1> a = { A(12) };
        std::array<int, 1> b{};

        INFO("`a` is moved inside zip object, its elements are copied to x.");
        for (auto [x, y] : zip(std::move(a), b)) {
            x.value = 4;
        }

        REQUIRE(A::copies() == 1);
        REQUIRE(A::moves() == 1);
        REQUIRE(a[0].value == 0); // NOLINT(bugprone-use-after-move)

        a = { A(12) };
        A::reset();

        INFO("`a` is moved inside zip object, its elements are moved to x.");
        for (auto [x, y] : zip<zip_config::MOVE_FROM_RVALUES>(std::move(a), b)) {
            x.value = 4;
        }

        REQUIRE(A::copies() == 0);
        REQUIRE(A::moves() == 2);
        REQUIRE(a[0].value == 0); // NOLINT(bugprone-use-after-move)
    }

    SECTION("auto & [x, y] = zip(move(a), b)"){
        using A = NewTypeCMC;

        std::array<A, 1> a = { A(0) };
        std::array<int, 1> b{};

        INFO("`a` is moved inside zip object, "
             "its elements are taken by reference.");
        for (auto & [x, y] : zip(std::move(a), b)) {
            x.value = 4;
        }

        REQUIRE(A::copies() == 0);
        REQUIRE(A::moves() == 1);
        REQUIRE(a[0].value == 0); // NOLINT(bugprone-use-after-move)
    }

    SECTION("auto && [x, y] = zip(move(a), b)"){
        using A = NewTypeCMC;

        std::array<A, 1> a = { A(0) };
        std::array<int, 1> b{};

        INFO("`a` is moved inside zip object, "
             "its elements are taken by reference.");
        for (auto && [x, y] : zip(std::move(a), b)) {
            x.value = 4;
        }

        REQUIRE(A::copies() == 0);
        REQUIRE(A::moves() == 1);
        REQUIRE(a[0].value == 0); // NOLINT(bugprone-use-after-move)
    }

    SECTION("auto [x, y] = zip(<initializer>, b)"){
        using A = NewTypeCMC;

        std::array<int, 1> b{};

        INFO("Collection is moved inside zip object, "
             "its elements are moved to x.");
        for (auto [x, y] : zip<zip_config::MOVE_FROM_RVALUES>(std::array<A, 1>{A(12)}, b)) {
            x.value = 4;
        }

        REQUIRE(A::copies() == 0);
        REQUIRE(A::moves() == 2);
    }

    SECTION("auto & [x, y] = zip(<initializer>, b)"){
        using A = NewTypeCMC;

        std::array<int, 1> b{};

        INFO("Collection is moved inside zip object, "
             "its elements are taken by reference.");
        for (auto & [x, y] : zip(std::array<A, 1>{A(12)}, b)) {
            x.value = 4;
        }

        REQUIRE(A::copies() == 0);
        REQUIRE(A::moves() == 1);
    }

    SECTION("auto && [x, y] = zip(<initializer>, b)"){
        using A = NewTypeCMC;

        std::array<int, 1> b{};

        INFO("Collection is moved inside zip object, "
             "its elements are taken by reference.");
        for (auto && [x, y] : zip(std::array<A, 1>{A(12)}, b)) {
            x.value = 4;
        }

        REQUIRE(A::copies() == 0);
        REQUIRE(A::moves() == 1);
    }
}

TEST_CASE("Modification", "[zip]") {
    std::vector u = {1, 2, 3, 4, 5};
    std::vector v = {'x', 'y', 'z'};

    INFO("Values are copied.");
    for (auto [x, y] : zip(u, v)) {
        x = y;
    }
    REQUIRE((v[0] != u[0] && v[1] != u[1] && v[2] != u[2]));

    INFO("Values are taken by reference.");
    for (auto& [x, y] : zip(u, v)) {
        x = y;
    }
    REQUIRE((v[0] == u[0] && v[1] == u[1] && v[2] == u[2]));
}

TEST_CASE("Containers", "[zip]") {
    SECTION("Basic containers") {
        std::vector v = {1, 2, 3, 0, 0, 0};
        std::set s = {2, 3, 4};
        std::map<int, std::string> m = {{2, "x"}, {4, "y"}, {8, "no"}};

        for (auto [x, y, z] : zip(v, s, m)) {
            STATIC_REQUIRE(std::is_const_v<decltype(y)>);
            STATIC_REQUIRE(std::is_const_v<decltype(z.first)>);
            REQUIRE((x + 1 == y && z.first == 1 << x));
        }
    }

    SECTION("Built-in arrays") {
        int A[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 1000};
        char B[] = {5, 4, 3, 2, 1};

        for (auto& [a, b] : zip(A, B)) {
            REQUIRE(a == 6 - b);
        }
    }

    SECTION("C-strings") {
        std::stringstream ss;

        auto message1 = "hello, world! ;)"sv;
        for (auto [x, y] : zip(message1, "...."sv)) {
            ss << x << y;
        }
        REQUIRE(ss.str() == "h.e.l.l.");
    }
}

TEST_CASE("Recursion", "[zip]") {
    std::vector v = {1, 2, 3};
    std::vector u = {3, 6, 9};

    std::stringstream ss;
    for (auto [x, y] : zip(zip(u, v), zip(u, v))) {
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

TEST_CASE("Collections", "[enumerate]") {
    SECTION("Initializer list") {
        auto numbers = {1u, 2u, 3u, 4u, 5u};
        for (auto [i, x] : enumerate(numbers)) {
            REQUIRE(i + 1 == x);
        }
    }

    SECTION("Vector") {
        std::vector v = {1, 2, 3, 4, 5};
        std::stringstream ss;

        auto enumeration = enumerate(v);
        for (auto [i, x] : enumeration) {
            ss << i << ' ' << x << ' ';
        }

        REQUIRE(ss.str() == "0 1 1 2 2 3 3 4 4 5 ");
    }

    SECTION("Set") {
        std::set s = {'a', 'b', 'c', 'd', 'e'};
        std::stringstream ss;

        auto enumeration = enumerate(s);
        for (auto [i, x] : enumeration) {
            ss << i << ' ' << x << ' ';
        }

        REQUIRE(ss.str() == "0 a 1 b 2 c 3 d 4 e ");
    }

    SECTION("Map") {
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

    SECTION("Built-ins") {
        std::vector<int> simple_array[] = {{2}, {4}, {8}, {16}};

        for (auto [i, x] : enumerate(simple_array)) {
            REQUIRE(static_cast<unsigned>(x[0]) == 1u << (i + 1));
        }
    }

    SECTION("Rvalue") {
        for (auto [i, x] : enumerate(std::vector<size_t>{0, 1, 2, 3, 4, 5, 6, 7, 8})) {
            REQUIRE(i == x);
        }
    }
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

    for (auto [idx, value] : indexed(0u, 1u, 2u)) {
        REQUIRE(idx == value);
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

TEST_CASE("Exceptions", "zip") {
    struct BadCollection: public std::vector<int> {
        using std::vector<int>::vector;
        BadCollection(BadCollection && other) noexcept(false) { }
    };
    std::initializer_list<int> list = {1, 2, 3, 4, 5};
    BadCollection collection = {1, 2, 3, 4, 5};
    STATIC_REQUIRE(noexcept(zip(list)));
    STATIC_REQUIRE(!noexcept(zip(std::move(collection))));
}

TEST_CASE("Zip returns forward range", "[zip]") {
    STATIC_REQUIRE(std::ranges::forward_range<decltype(zip("str"sv))>);
}

TEST_CASE("Strong exception guarantee", "[zip]") {
    struct It {
        using value_type = int;
        using reference = int&;
        using iterator_category = std::forward_iterator_tag;
        using difference_type = int;

        It() = default;
        explicit It(int x): i{x} { }
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


TEST_CASE("Enumerate elems") {
    for (auto [i, val] : indexed(0u, 1u, 2u, 3u, 4u, 5u)) {
        REQUIRE(i == val);
    }
}