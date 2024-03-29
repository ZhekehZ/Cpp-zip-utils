## Cpp-zip-utils [c++20] ##
[![CMake](https://github.com/ZhekehZ/Cpp-zip-utils/actions/workflows/cmake.yml/badge.svg?branch=master)](https://github.com/ZhekehZ/Cpp-zip-utils/actions/workflows/cmake.yml)
[![Conan version - ZipUtils/0.1@zhekehz/stable](https://img.shields.io/badge/Conan_version-ZipUtils%2F0.1%40zhekehz%2Fstable-6699cb?logo=conan&logoColor=6699cb)](https://https://zhekehz.jfrog.io/artifactory/api/conan/zhekehz-conan)
[![20](https://img.shields.io/badge/20-red?logo=c%2B%2B)](https://)  

_Some structured_binding-friendly functions for `C++`_

---
### Installation ###
- via CMake
    ```bash
    mkdir -p build && \
    cmake -B build && \
    cmake --build build --target test && \
    sudo cmake --build build --target install
    ```
- via Conan
    1. add Conan remote
        ```bash
        conan remote add zhekehz-conan https://zhekehz.jfrog.io/artifactory/api/conan/zhekehz-conan
        ```
    2. Configure CMake:
        ```cmake
        conan_cmake_run(REQUIRES ZipUtils/0.1@zhekehz/stable)
        ```
    see the [exmaple subproject](example) for more details
---
### Declaration ###
```c++
template <std::ranges::forward_range ... Containers>
auto zip(Containers && ... containers);
```

```c++
template <std::ranges::forward_range ... Containers>
auto enumerate(Containers && ... containers);
```

```c++
template <typename Value, std::same_as<Value> ... Values>
constexpr auto indexed(Value && value, Values && ... values);
```
---
### Usage ###
```c++
#include <cassert>

#include "zip_utils.hpp"
using namespace zip_utils::views;
/*  or
    using zip_utils::views::zip;
    using zip_utils::views::enumerate;
    using zip_utils::views::indexed;
*/

void test_10_fibonacci() {    
    std::array<int, 10> F = {0, 1};
    auto expected = {0, 1, 1, 2, 3, 5, 8, 13, 21, 34};

    using std::views::drop;

    // zip view
    for (auto & [F0, F1, F2] : zip(F, F | drop(1), F | drop(2))) {
        F2 = F0 + F1;
    }

    // enumerate view
    for (auto [i, x] : enumerate(expected)) {
        assert(F[i] == x);
    }
    
    // indexed view
    for (auto [idx, value] : indexed(0u, 1u, 2u)) {
        assert(idx == value);
    }
}
```
---
#### Features ####
*   **Zip**
    *   Simple iteration
        ```c++
        std::vector<int> v = { /* */ };
        std::set<char>   s = { /* */ };
        
        for (auto const & [x, y] : zip(v, s)) { 
            std::cout << "x = " << x << ", y = " << y << std::endl; 
        }
        ```
      
    *   Container modification
        ```c++
        std::vector<int> v = { /* */ };
        std::vector<int> v_half(v.size() / 2); 
        
        for (auto & [x, y] : zip(v, v_half)) {
            y = x;
        }
        ```

    *   PRValues / XValues
        ```c++
        std::vector<A> data(3);

        // Default behaviour
        for (auto [x, y] : zip(std::move(data), /* ... */)) {
            // data will be moved into zip object
            // elements will be **copied** into `x`
        }
        
        // If you want to iterate just once
        using zip_utils::configuration::zip_config;
        for (auto [x, y] : zip<zip_config::MOVE_FROM_RVALUES>(std::move(data), /* ... */) {
            // data will be moved into zip object
            // elements will be **moved** into `x`
        }
        ```

    *   Built-in arrays and c-strings
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
      
    *   Zip result is also `forward_range`
        ```c++
        std::vector<int> v(0);
        
        for (auto [y, x] : zip(zip(v, v), zip(v, v))) {
            auto [t, w] = y;     
            /* */
        }
        ```
      
    *   Constexpr
        ```c++
        constexpr auto sum = [] (const auto & array) -> int {
            int sum = 0;
            for (auto const & [x] : zip(array)) sum += x;  
            return sum;
        };
        
        constexpr std::array<int, 5> arr = {1, 43, 7, 3, 7};
        static_assert(sum(arr) == 61);
        ```

*   **Enumerate**
    ```c++
    std::vector<int> v = { /* */ };
        
    for (auto [i, x] : enumerate(v)) {
        std::cout << "v[" << i << "] == " << x << std::endl;
    }
    ```
*   **Indexed**
    ```c++
    for  (auto [i, x] : indexed(0, 1, 2)) {
        std::cout << "index = " << i << ", value = " << x << std::endl;
    }
    ```
