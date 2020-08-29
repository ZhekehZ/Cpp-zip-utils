#pragma once

#include <concepts>
#include <iterator>
#include <tuple>

namespace zip_utils {

    template<typename C>
    concept forward_iterable = requires(C const & iterable) {
        { std::begin(iterable) } -> std::forward_iterator;
        { std::end(iterable) } -> std::forward_iterator;
    };

    namespace detail {

        template<std::forward_iterator ... Iterators>
        class iterator_pack : public std::tuple<Iterators...> {
        public:
            using base = std::tuple<Iterators...>;

            using base::tuple;

            template<std::size_t I>
            auto &get() &{
                return *std::get<I>(static_cast<base>(*this));
            }

            template<std::size_t I>
            [[nodiscard]] const auto &get() const &{
                return *std::get<I>(static_cast<base>(*this));
            }

            template<std::size_t I>
            auto get() &&{
                return *std::get<I>(static_cast<base>(*this));
            }

            template<std::size_t I>
            [[nodiscard]] auto get() const &&{
                return *std::get<I>(static_cast<base>(*this));
            }
        };

    } // detail
} // zip_utils

namespace std {

    template <typename ... Iterators>
    struct tuple_size<zip_utils::detail::iterator_pack<Iterators...>> {
        static constexpr size_t value = sizeof ... (Iterators);
    };

    template <std::size_t I, std::forward_iterator ... Iterators>
    struct tuple_element<I, zip_utils::detail::iterator_pack<Iterators...>> {
        using iterator = tuple_element_t<I, tuple<Iterators...>>;
        using reference = typename iterator_traits<iterator>::reference;
        using type = std::remove_reference_t<reference>;
    };

    template <std::size_t I, std::forward_iterator ... Iterators>
    struct tuple_element<I, const zip_utils::detail::iterator_pack<Iterators...>> {
        using type = const tuple_element_t<I, zip_utils::detail::iterator_pack<Iterators...>>;
    };

} // std


namespace zip_utils {

    namespace detail {

        template<std::forward_iterator ... Iterators>
        class zip_iterator {
        public:
            using value_type = iterator_pack<Iterators...>;
            using reference = value_type &;
            using iterator_category = std::forward_iterator_tag;
            using difference_type = int;

            zip_iterator() = default;

            explicit zip_iterator(Iterators ... iterators) : iterator(iterators...) {}

            reference operator*() const {
                return iterator;
            }

            zip_iterator &operator++() {
                typename iterator_pack<Iterators...>::base & tup = iterator;
                [&]<std::size_t ... I>(std::index_sequence<I ...>) {
                    (++std::get<I>(tup), ...);
                }(std::make_index_sequence<sizeof...(Iterators)>{});
                return *this;
            }

            zip_iterator operator++(int) {
                auto copy = *this;
                ++*this;
                return copy;
            }

            bool operator==(zip_iterator const &other) const {
                typename iterator_pack<Iterators...>::base & c_tup = iterator;
                typename iterator_pack<Iterators...>::base & o_tup = other.iterator;
                return [&]<std::size_t ... I>(std::index_sequence<I...>) {
                    return ((std::get<I>(c_tup) == std::get<I>(o_tup)) || ...);
                }(std::make_index_sequence<sizeof...(Iterators)>{});
            }

            bool operator!=(zip_iterator const &other) const {
                return !(*this == other);
            }

        private:

            mutable iterator_pack<Iterators...> iterator;
        };

        static_assert(std::forward_iterator<zip_iterator<int *>>);

        template<std::forward_iterator ... Iterators>
        class zip_impl {
            static_assert(sizeof...(Iterators) > 0, "AT_LEAST_ONE_ARGUMENT_NEEDED");
            using Iter = zip_iterator<Iterators...>;
        public:
            zip_impl(Iter begin, Iter end)
                    : begin_(std::move(begin)), end_(std::move(end)) {}

            auto begin() const {
                return begin_;
            }

            auto end() const {
                return end_;
            }

        private:
            Iter begin_;
            Iter end_;
        };

    } // detail

    template <forward_iterable ... Containers>
    auto zip(Containers && ... containers) {
        using namespace detail;
        return zip_impl{zip_iterator{std::begin(std::forward<Containers>(containers))...},
                        zip_iterator{std::end(std::forward<Containers>(containers))...}};
    }

    inline auto operator""_sw(const char * str, std::size_t len) {
        return std::string_view(str, len);
    }

    namespace detail {

        class counting_iterator {
        public:
            using value_type = const std::size_t;
            using reference = value_type &;
            using iterator_category = std::forward_iterator_tag;
            using difference_type = int;

            explicit counting_iterator(std::size_t start = 0) : index_(start) {}

            reference operator*() const {
                return index_;
            }

            counting_iterator &operator++() {
                ++index_;
                return *this;
            }

            counting_iterator operator++(int) {
                auto copy = *this;
                ++*this;
                return copy;
            }

            bool operator==(counting_iterator const &) const {
                return false;
            }

            bool operator!=(counting_iterator const &) const {
                return true;
            }

        private:
            std::size_t index_;
        };

        class counter {
        public:
            explicit counter(std::size_t start = 0) : start_(start) {}

            [[nodiscard]] counting_iterator begin() const {
                return counting_iterator{start_};
            }

            [[nodiscard]] counting_iterator end() const {
                return counting_iterator();
            }

        private:
            std::size_t start_;
        };

        static_assert(std::forward_iterator<counting_iterator>);
        static_assert(forward_iterable < counter > );

    } // detail

    template <forward_iterable ... Containers>
    auto enumerate(Containers && ... containers) {
        using namespace detail;
        return zip(counter{}, std::forward<Containers>(containers) ...);
    }

    template <typename T>
    auto enumerate(std::initializer_list<T> && list) {
        return enumerate(list);
    }

} // zip_utils
