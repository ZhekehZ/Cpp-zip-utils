# Cpp-zip-utils [![Build Status](https://travis-ci.com/ZhekehZ/Cpp-zip-utils.svg?branch=master)](https://travis-ci.com/ZhekehZ/Cpp-zip-utils)
#### _Some structured_binding-friendly functions for `C+-`_

---

#### Declaration:
```c++
template <std::ranges::forward_range ... Containers>
auto zip(Containers && ... containers);
```

```c++
template <std::ranges::forward_range ... Containers>
auto enumerate(Containers && ... containers);
```
---
#### Usage:
```c++
#include <cassert>

#include "zip_utils.hpp"
using namespace zip_utils;
/*  or
    using zip_utils::zip;
    using zip_utils::enumerate;
    using zip_utils::skip;
    using zip_utils::operator""_sw;
*/

void test_10_fibonacci() {    
    std::array<int, 10> F = {0, 1};

    auto fibs_iterator = zip(F, F, F).skip<1>(1).skip<2>(2);
    auto expected = enumerate({0, 1, 1, 2, 3, 5, 8, 13, 21, 34});
    
    for (auto & [F0, F1, F2] : fibs_iterator) {
        F2 = F0 + F1;
    }

    for (auto [i, x] : expected) {
        assert(F[i] == x);
    }
}
```
---
#### Features:
*  **Zip**
    - Simple iteration
        ```c++
        std::vector<int> v = { /* */ };
        std::set<char>   s = { /* */ };
        
        auto zipped = zip(v, s);
        for (auto const & [x, y] : zipped) { 
            std::cout << "x = " << x << ", y = " << y << std::endl; 
        }
        ```
    - Container modification
        ```c++
        std::vector<int> v = { /* */ };
        std::vector<int> v_half(v.size() / 2); 
      
        auto zipped = zip(v, v_half);
        for (auto & [x, y] : zipped) {
            y = x;
        }
        ```
    - Built-in arrays and c-strings
        ```c++
        int a[4] = {0};
        auto s = "hello, world";
      
        auto zipped = zip(a, s);
        for (auto [x, y] : zipped) {
            std::cout << y;  
        }
      
        // Note: you can use `_sw` suffix (sw stands for std::string_view) for c-strings 
        //    to avoid terminating zero:
        auto zipped = zip("abc"_sw); 
        for (auto const & [x] : zipped) { /* x in [a b c] */ }
        ```
    - Zip result is also `forward_range`
        ```c++
        std::vector<int> v(0);
      
        auto zipped = zip(zip(v, v), v);
        for (auto [y, x] : zipped) {
            auto [t, w] = y;     
            /* */
        }
        ```
    - Constexpr
        ```c++
        constexpr auto sum = [] (const auto & array) -> int {
            int sum = 0;
      
            auto zipped = zip(array);
            for (auto const & [x] : zipped) sum += x;  
            return sum;
        };
      
        constexpr std::array<int, 5> arr = {1, 43, 7, 3, 7};
        static_assert(sum(arr) == 61);
        ```
    - Skip
        ```c++
        std::vector v = {1, 2, 3, 4, 5};
      
        auto zipped = zip(v, v).skip<0>(1);
        for (auto [x, y] : zipped) {
            // skip<i, j> skips j iterations of the i-th iterator
            // x == 2, 3, 4, 5
            // y == 1, 2, 3, 4, 5
        }
        ```
      
*   **Enumerate**
    - Simple enumeration
        ```c++
        std::vector<int> v = { /* */ };
      
        auto enumeration = enumerate(v);
        for (auto [i, x] : enumeration) {
            std::cout << "v[" << i << "] == " << x << std::endl;
        }
        ```
    - Initializer list
        ```c++
        auto enumeration = enumerate({0, 1, 2, 3, 4, 5});
        for (auto [i, x] : enumeration) {
            assert(i == x);
        }
        ```
