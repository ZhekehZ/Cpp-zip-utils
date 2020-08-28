# Cpp-enumerate
##### My python-style `enumerate` implementation for C++

---

#### Declaration:
```c++
template<typename C>
concept iterable = requires(C const &iterable) {
    { std::begin(iterable) } -> std::forward_iterator;
    { std::end(iterable) } -> std::forward_iterator;
};

template <iterable Container>
auto enumerate(Container & container); // yields std::pair<const std::size_t, element_ref>
                                       //     or std::pair<const std::size_t, element_cref>

template <iterable Container>
auto enumerate(Container const & container); // yields std::pair<const std::size_t, element_cref>
```

---

#### Features:

* Simple iteration
```c++
std::vector<int> v = {5, 6, 7, 10};

for (auto const [i, x] : enumerate(v)) {
    std::cout << "v[" << i << "] = " << x << std::endl;
}

/* v[0] = 5
   v[1] = 6
   v[2] = 7
   v[3] = 10
*/
```

* Modification:
```c++
std::vector<int> v = {5, 6, 7, 10};

for (auto [i, x] : enumerate(v)) {
    x += i;
}

// now v == {0, 7, 9, 13}
```

* Built-in arrays and C-strings
```c++
int array[] = {2, 3, 8};
for (auto [j, x] : enumerate(array)) {
    /* CODE */
}

for (auto const [i, c] : enumerate("hello")) {
    /* CODE */
}
```

* Initializer list
```c++
for (auto [i, x] : enumerate({1, 2, 3})) {
    std::cout << x << ' ' ;
}
```