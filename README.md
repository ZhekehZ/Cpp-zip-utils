# Cpp-zip-utils [![CMake](https://github.com/ZhekehZ/Cpp-zip-utils/actions/workflows/cmake.yml/badge.svg)](https://github.com/ZhekehZ/Cpp-zip-utils/actions/workflows/cmake.yml) [![CodeQL](https://github.com/ZhekehZ/Cpp-zip-utils/actions/workflows/codeql-analysis.yml/badge.svg?branch=other_add_code_analysis)](https://github.com/ZhekehZ/Cpp-zip-utils/actions/workflows/codeql-analysis.yml)
#### _Some structured_binding-friendly functions for `C+-`_

---

#### Declaration:
```c++
template <std::ranges::forward_range ... Containers>
auto zip(Containers & ... containers);
```

```c++
template <std::ranges::forward_range ... Containers>
auto enumerate(Containers & ... containers);
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
*/

void test_10_fibonacci() {    
    std::array<int, 10> F = {0, 1};
    auto expected = {0, 1, 1, 2, 3, 5, 8, 13, 21, 34};
    
    for (auto & [F0, F1, F2] : zip(F, F, F).skip<1>(1).skip<2>(2)) {
        F2 = F0 + F1;
    }

    for (auto [i, x] : enumerate(expected)) {
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
        
        for (auto const & [x, y] : zip(v, s)) { 
            std::cout << "x = " << x << ", y = " << y << std::endl; 
        }
        ```
    - Container modification
        ```c++
        std::vector<int> v = { /* */ };
        std::vector<int> v_half(v.size() / 2); 
      
        for (auto & [x, y] : zip(v, v_half)) {
            y = x;
        }
        ```
    - Built-in arrays and c-strings
        ```c++
        int a[4] = {0};
        auto s = "hello, world";
      
        for (auto [x, y] : zip(a, s)) {
            std::cout << y;  
        }


        using namespace std::literals;
        // Note: you can use `sv` suffix for c-strings 
        //    to avoid terminating zero:
        auto abc = "abc"sv; 
        for (auto const & [x] : zip(abc)) { /* x in [a b c] */ }
        ```
    - Zip result is also `forward_range`
        ```c++
        std::vector<int> v(0);
      
        auto zip_vv = zip(v, v);
        for (auto [y, x] : zip(zip_vv, zip_vv)) {
            auto [t, w] = y;     
            /* */
        }
        ```
    - Constexpr
        ```c++
        constexpr auto sum = [] (const auto & array) -> int {
            int sum = 0;
            for (auto const & [x] : zip(array)) sum += x;  
            return sum;
        };
      
        constexpr std::array<int, 5> arr = {1, 43, 7, 3, 7};
        static_assert(sum(arr) == 61);
        ```
    - Skip
        ```c++
        std::vector v = {1, 2, 3, 4, 5};
      
        for (auto [x, y] : zip(v, v).skip<0>(1)) {
            // skip<i, j> skips j iterations of the i-th iterator
            // x == 2, 3, 4, 5
            // y == 1, 2, 3, 4, 5
        }
        ```
      
*   **Enumerate**
    - Simple enumeration
        ```c++
        std::vector<int> v = { /* */ };
      
        for (auto [i, x] : enumerate(v)) {
            std::cout << "v[" << i << "] == " << x << std::endl;
        }
        ```
