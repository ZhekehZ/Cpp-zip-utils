# Cpp-zip-utils [![Build Status](https://travis-ci.com/ZhekehZ/Cpp-enumerate.svg?token=ML5sxGgAoF8mdBb9oR6a&branch=master)](https://travis-ci.com/ZhekehZ/Cpp-enumerate)
#### _Some structured_binding-friendly functions for `C+-`_

---

#### Declaration:
```c++
// Containers must meet the following concept:
template<typename C>
concept forward_iterable = requires(C const &forward_iterable) {
    { std::begin(forward_iterable) } -> std::forward_iterator;
    {  std::end(forward_iterable)  } -> std::forward_iterator;
};
```

```c++
template <forward_iterable ... Containers>
auto zip(Containers && ... containers);
```

```c++
template <forward_iterable ... Containers>
auto enumerate(Containers && ... containers);
```
---
#### Usage:
```c++
#include "zip_utils.hpp"
using namespace zip_utils;
/*  or
    using zip_utils::zip;
    using zip_utils::enumerate;
    using zip_utils::operator""_sw;
*/
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
      
        for (auto & [x, y] : zip(v, v_half) {
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
      
        // Note: you can use `_sw` suffix (sw stands for std::string_view) for c-strings 
        //    to avoid terminating zero: 
        for (auto const & [x] : zip("abc"_sw)) { /* x in [a b c] */ }
        ```
    - Zip result is also forward_iterable
        ```c++
        std::vector<int> v(0);
      
        for (auto [y, x] : zip(zip(v, v), v)) {
            auto [t, w] = y;     
            /* */
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
    - Initializer list
        ```c++
        for (auto [i, x] : enumerate({0, 1, 2, 3, 4, 5})) {
            assert(i == x);
        }
        ```