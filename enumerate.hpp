#pragma once

#include <concepts>
#include <iterator>

template<typename C>
concept iterable = requires(C const &iterable) {
    { std::begin(iterable) } -> std::forward_iterator;
    { std::end(iterable) } -> std::forward_iterator;
};

template<iterable Container>
auto enumerate(Container && container);

namespace detail {
    class non_copyable_non_movable {
    public:
        non_copyable_non_movable() = default;

        non_copyable_non_movable(non_copyable_non_movable const &) = delete;

        non_copyable_non_movable(non_copyable_non_movable &&) = delete;

        non_copyable_non_movable &operator=(non_copyable_non_movable const &) = delete;

        non_copyable_non_movable &operator=(non_copyable_non_movable &&) = delete;
    };

    template<typename Iter>
    struct xvalue_pair;

    template<typename Iter>
    class implicit_reference_wrapper : public non_copyable_non_movable {
        using T = std::remove_reference_t<typename std::iterator_traits<Iter>::reference>;
    public:
        operator T &() { return *data; }

        operator T const &() const { return *data; }

        template<typename U>
        requires std::is_assignable_v<T &, U>
        auto operator=(U &&value) {
            return *data = std::forward<U>(value);
        }

    private:
        friend struct xvalue_pair<Iter>;

        [[maybe_unused]] explicit implicit_reference_wrapper(Iter data) : data(data) {}

        Iter data;
    };


    template<typename Iter>
    struct xvalue_pair {
        xvalue_pair(const size_t index, Iter it) : index(index), wrapper(it) {}

        std::size_t index;
        implicit_reference_wrapper<Iter> wrapper;
    };


    template<typename Iter>
    class enumeration_iterator : non_copyable_non_movable {
    public:
        explicit enumeration_iterator(Iter outer_iterator)
                : outer(outer_iterator), index{0} {}

        friend bool operator==(enumeration_iterator const &a, enumeration_iterator const &b) {
            return a.outer == b.outer;
        }

        friend bool operator!=(enumeration_iterator const &a, enumeration_iterator const &b) {
            return a.outer != b.outer;
        }

        auto &operator++() {
            ++outer;
            ++index;
            return *this;
        }

        auto operator++(int) {
            auto copy = *this;
            ++*this;
            return copy;
        }

        auto operator*() {
            return xvalue_pair(index, outer);
        }

    private:
        Iter outer;
        std::size_t index;
    };


    template<typename ContainerRef>
    class enumeration_impl : public non_copyable_non_movable {
    public:
        [[maybe_unused]] friend auto begin(enumeration_impl &enumerator) {
            return enumeration_iterator(std::begin(enumerator.container));
        }

        [[maybe_unused]] friend auto end(enumeration_impl &enumerator) {
            return enumeration_iterator(std::end(enumerator.container));
        }

    private:
        explicit enumeration_impl(ContainerRef container) : container(container) {}

        template<iterable Container>
        friend auto::enumerate(Container &&container);

        ContainerRef container;
    };

}

template<iterable Container>
auto enumerate(Container && container) {
    return detail::enumeration_impl<Container>{container};
}

template<typename T>
auto enumerate(std::initializer_list<T> && init_list) {
    return enumerate<std::initializer_list<T>>(std::move(init_list));
}