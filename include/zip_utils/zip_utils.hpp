#pragma once

#include <concepts>
#include <iterator>
#include <tuple>
#include <ranges>

namespace zip_utils::detail {

    template <typename Tuple, std::size_t I>
    static constexpr bool nothrow_deref_tuple_element = requires {
        requires requires(std::tuple_element_t<I, Tuple> & t) {
            { *t } noexcept;
        };
    };

    template <typename T>
    static constexpr bool nothrow_comparable = requires (const T & t) {
        { t == t } noexcept;
    };

    template <typename T>
    static constexpr bool nothrow_incrementable = requires (T & t) {
        { ++t } noexcept;
    };

    template<std::forward_iterator ... Iterators>
    class iterator_pack : public std::tuple<Iterators...> {
    public:
        using base = std::tuple<Iterators...>;
        using base::tuple;

        template <std::size_t I>
        constexpr auto &get() & noexcept (nothrow_deref_tuple_element<base, I>) {
            return *std::get<I>(static_cast<base>(*this));
        }

        template <std::size_t I>
        [[nodiscard]] constexpr const auto & get() const &
            noexcept (nothrow_deref_tuple_element<const base, I>)
        {
            return *std::get<I>(static_cast<base>(*this));
        }

        template <std::size_t I>
        constexpr auto get() &&  noexcept (nothrow_deref_tuple_element<base, I>) {
            return *std::get<I>(static_cast<base>(*this));
        }

        template <std::size_t I>
        [[nodiscard]] constexpr auto get() const &&
            noexcept (nothrow_deref_tuple_element<const base, I>)
        {
            return *std::get<I>(static_cast<base>(*this));
        }

        constexpr bool operator==(iterator_pack const & other) const
            noexcept ((nothrow_comparable<Iterators> && ...))
        {
            const base & self = *this;
            const base & that = other;
            return [&]<std::size_t ... I>(std::index_sequence<I...>) {
                return ((std::get<I>(self) == std::get<I>(that)) || ...);
            }(std::make_index_sequence<sizeof...(Iterators)>{});
        }

        constexpr bool operator!=(iterator_pack const & other) const
            noexcept ((nothrow_comparable<Iterators> && ...))
        {
            return !(*this == other);
        }

        template <std::size_t I>
        constexpr void next(std::size_t val)
            noexcept (requires (base & ip) { { ip = std::next(std::get<I>(ip), val) } noexcept; })
        {
            base & self = *this;
            auto & it = std::get<I>(self);
            it = std::next(it, val);
        }

        constexpr void inc()
            noexcept ((nothrow_incrementable<Iterators> && ...))
        {
            auto inc = [&]<std::size_t ... I>(std::index_sequence<I ...>, base & tup) {
                (++std::get<I>(tup), ...);
            };

            // Strong exception guarantee
            if constexpr ((nothrow_incrementable<Iterators> && ...)) {
                inc(std::make_index_sequence<sizeof...(Iterators)>{}, *this);
            } else {
                base self = *this;
                inc(std::make_index_sequence<sizeof...(Iterators)>{}, self);
                static_cast<base&>(*this) = self;
            }
        }
    };

} // zip_utils::detail

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
            using IterPack = iterator_pack<Iterators...>;

            constexpr zip_iterator() noexcept = default;

            constexpr explicit zip_iterator(Iterators ... iterators)
                noexcept (noexcept (IterPack(iterators...)))
                : iterator(iterators...)
            {}

            constexpr reference operator*() const noexcept {
                return const_cast<reference>(iterator);
            }

            constexpr zip_iterator & operator++() noexcept (noexcept(this->iterator.inc())) {
                iterator.inc();
                return *this;
            }

            constexpr zip_iterator operator++(int)
                noexcept (noexcept(zip_iterator(++*this)))
            {
                auto copy = *this;
                ++*this;
                return copy;
            }

            constexpr bool operator==(zip_iterator const & other) const
                noexcept (noexcept(iterator == iterator))
            {
                return iterator == other.iterator;
            }

            constexpr bool operator!=(zip_iterator const & other) const
                noexcept (noexcept(iterator == iterator))
            {
                return iterator != other.iterator;
            }

            template <std::size_t I>
            constexpr void next(std::size_t val)
                noexcept (noexcept(this->iterator.template next<I>(val)))
            {
                iterator.template next<I>(val);
            }

        private:

            IterPack iterator;
        };


        static_assert(std::forward_iterator<zip_iterator<int *>>);

        template<std::forward_iterator ... Iterators>
        class zip_impl {
            static_assert(sizeof...(Iterators) > 0, "AT_LEAST_ONE_ARGUMENT_NEEDED");
            using Iter = zip_iterator<Iterators...>;
        public:
            constexpr zip_impl(Iter begin, Iter end)
                noexcept (std::is_nothrow_move_assignable_v<Iter>)
                : begin_(std::move(begin))
                , end_(std::move(end))
            {}

            constexpr auto begin() const noexcept
                (std::is_nothrow_copy_constructible_v<Iter>)
            {
                return begin_;
            }

            constexpr auto end() const noexcept
                (std::is_nothrow_copy_constructible_v<Iter>)
            {
                return end_;
            }

            template<std::size_t I>
            constexpr zip_impl & skip(std::size_t v) {
                begin_.template next<I>(v);
                return *this;
            }

        private:
            Iter begin_;
            Iter end_;
        };

        template <typename T>
        static constexpr bool is_nothrow_beg_end_copy_constructible_v = requires (T && arg) {
            requires noexcept ( std::begin(std::forward<T>(arg)) );
            requires noexcept ( std::end(std::forward<T>(arg)) );
            requires std::is_nothrow_copy_constructible_v<
                std::remove_cvref_t<decltype(std::begin(arg))>>;
        };

    } // detail

    template <std::ranges::forward_range ... Containers>
    constexpr auto zip(Containers && ... containers)
        noexcept ((detail::is_nothrow_beg_end_copy_constructible_v<Containers> && ...))
    {
        using namespace detail;
        return zip_impl{zip_iterator{std::begin(std::forward<Containers>(containers))...},
                        zip_iterator{std::end(std::forward<Containers>(containers))...}};
    }

    constexpr inline auto operator""_sw(const char * str, std::size_t len) {
        return std::string_view(str, len);
    }

    namespace detail {

        class counting_iterator {
        public:
            using value_type = const std::size_t;
            using reference = value_type &;
            using iterator_category = std::forward_iterator_tag;
            using difference_type = int;

            constexpr reference operator*() const noexcept {
                return index_;
            }

            constexpr counting_iterator & operator++() noexcept {
                ++index_;
                return *this;
            }

            constexpr counting_iterator operator++(int) noexcept {
                auto copy = *this;
                ++*this;
                return copy;
            }

            constexpr bool operator==(counting_iterator const &) const noexcept {
                return false;
            }

            constexpr bool operator!=(counting_iterator const &) const noexcept {
                return true;
            }

        private:
            std::size_t index_;
        };

        static constexpr counting_iterator end_count{};

        struct counter {
            [[nodiscard]] constexpr static counting_iterator begin() noexcept {
                return counting_iterator{};
            }

            [[nodiscard]] constexpr static counting_iterator end() noexcept {
                return end_count;
            }
        };

        static_assert(std::forward_iterator<counting_iterator>);
        static_assert(std::ranges::forward_range<counter>);

    } // detail

    template <std::ranges::forward_range ... Containers>
    constexpr auto enumerate(Containers && ... containers)
        noexcept (noexcept(zip(std::forward<Containers>(containers) ...)))
    {
        using namespace detail;
        return zip(counter{}, std::forward<Containers>(containers) ...);
    }

    template <typename T>
    constexpr auto enumerate(std::initializer_list<T> && list)
        noexcept (noexcept(enumerate(list)))
    {
        return enumerate(list);
    }

} // zip_utils
