#pragma once

#include <concepts>
#include <iterator>


template<typename C>
concept iterable = requires(C const &iterable) {
    { std::begin(iterable) } -> std::forward_iterator;
    { std::end(iterable) } -> std::forward_iterator;
};


template <iterable Container>
auto enumerate(Container & container);
template <iterable Container>
auto enumerate(Container const & container);


namespace detail {
    struct non_copyable_non_movable {
        non_copyable_non_movable() = default;
        non_copyable_non_movable(non_copyable_non_movable const &) = delete;
        non_copyable_non_movable(non_copyable_non_movable &&) = delete;
        non_copyable_non_movable &operator=(non_copyable_non_movable const &) = delete;
        non_copyable_non_movable &operator=(non_copyable_non_movable &&) = delete;
    };

    template<std::forward_iterator Iter>
    class enumeration_iterator;

    template<std::forward_iterator Iter>
    class implicit_reference_wrapper : public non_copyable_non_movable {
        // type with const-ness
        using T = std::remove_reference_t<typename std::iterator_traits<Iter>::reference>;

    public:
        explicit implicit_reference_wrapper(Iter data) : data_(data) {}

        operator T & () { return *data_; }
        operator T const & () const { return *data_; }

        template<typename U>
        requires std::is_assignable_v<T &, U>
        auto operator=(U && value) {
            return *data_ = std::forward<U>(value);
        }

    private:

        Iter data_;
    };


    template <std::forward_iterator Iter>
    class enumeration_iterator : non_copyable_non_movable {
    public:
        using self_type = enumeration_iterator<Iter>;
        using reference = std::pair<const std::size_t, implicit_reference_wrapper<Iter>>;
        using pointer = typename std::iterator_traits<Iter>::pointer;
        using iterator_category = std::forward_iterator_tag;

        explicit enumeration_iterator(Iter outer_iterator)
            : outer_(outer_iterator)
            , index_{0}
        {}

        self_type & operator++() {
            ++outer_;
            ++index_;
            return *this;
        }

        self_type operator++(int) {
            auto tmp = *this;
            ++*this;
            return tmp;
        }

        reference operator*() {
            return reference(index_, outer_);
        }

        pointer operator->() {
            return outer_.operator->();
        }

        bool operator==(enumeration_iterator const & other) const {
            return outer_ == other.outer_;
        }

        bool operator!=(enumeration_iterator const & other) const {
            return outer_ != other.outer_;
        }

    private:
        Iter outer_;
        std::size_t index_;
    };


    template <std::forward_iterator Iter>
    class enumeration_impl : public non_copyable_non_movable {
    public:
        auto begin() { return enumeration_iterator(begin_); }
        auto end() { return enumeration_iterator(end_); }

    private:
        explicit enumeration_impl(Iter begin, Iter end)
            : begin_(begin)
            , end_(end)
        {}

        template <iterable Container>
        friend auto ::enumerate(Container & container);
        template <iterable Container>
        friend auto ::enumerate(Container const & container);

        Iter begin_;
        Iter end_;
    };

}

template <iterable Container>
auto enumerate(Container & container) {
    return detail::enumeration_impl{std::begin(container), std::end(container)};
}

template <iterable Container>
auto enumerate(Container const & container) {
    return detail::enumeration_impl{std::begin(container), std::end(container)};
}

template<typename T>
auto enumerate(std::initializer_list<T> init_list) {
    return enumerate<std::initializer_list<T>>(std::move(init_list));
}
