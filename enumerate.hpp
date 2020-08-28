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
auto enumerate(const char * str);

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
    class reference_holder : public non_copyable_non_movable {
    public:
        // type with const-ness
        using T = typename std::iterator_traits<Iter>::reference;
        friend class enumeration_iterator<Iter>;

        const std::size_t index_;
        T ref_;

    private:
        reference_holder(size_t index, T ref)
                : index_(index)
                , ref_(ref)
        {}
    };


    template <std::forward_iterator Iter>
    class enumeration_iterator : non_copyable_non_movable {
    public:
        using self_type = enumeration_iterator<Iter>;
        using reference = reference_holder<Iter>;
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
            return reference{index_, *outer_};
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
        friend auto ::enumerate(const char * str);

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

auto enumerate(const char * str) {
    return detail::enumeration_impl{str, str + strlen(str) - 1};
}

template<typename T>
auto enumerate(std::initializer_list<T> && init_list) {
    return enumerate<std::initializer_list<T>>(std::move(init_list));
}
