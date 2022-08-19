#pragma once

#include <concepts>
#include <cstddef>
#include <exception>
#include <tuple>
#include <type_traits>

#include "configuration.hpp"
#include "utils.hpp"


namespace zip_utils::detail::impl {


    template <configuration::zip_config Config, typename Mask, std::forward_iterator... Iterators>
    class zip_value : private std::tuple<Iterators...> {
       private:
        using base = std::tuple<Iterators...>;

       public:
        using base::base;

        constexpr void increment() noexcept((noexcept(++std::get<Iterators>(static_cast<base &>(*this))) && ...)) {
            if ((noexcept(++std::get<Iterators>(static_cast<base &>(*this))) && ...)) {
                // noexcept
                [&]<std::size_t... Indices>(std::index_sequence<Indices...>) {
                    (++std::get<Indices>(static_cast<base &>(*this)), ...);
                }
                (std::make_index_sequence<sizeof...(Iterators)>{});
            } else {
                // strong exception guarantee
                static_assert(std::is_nothrow_move_assignable_v<zip_value>,
                              "One or more iterators are not nothrow assignable.");

                zip_value copy = *this;
                try {
                    [&]<std::size_t... Indices>(std::index_sequence<Indices...>) {
                        (++std::get<Indices>(static_cast<base &>(*this)), ...);
                    }
                    (std::make_index_sequence<sizeof...(Iterators)>{});
                } catch (...) {
                    *this = std::move(copy);
                    std::rethrow_exception(std::current_exception());
                }
            }
        }

        constexpr bool equals(zip_value const &other) const
            noexcept((noexcept(std::get<Iterators>(static_cast<base const &>(*this)) ==
                               std::get<Iterators>(static_cast<base const &>(*this))) &&
                      ...)) {
            auto &self = static_cast<base const &>(*this);
            auto &that = static_cast<base const &>(other);
            return [&]<std::size_t... Indices>(std::index_sequence<Indices...>) {
                return ((std::get<Indices>(self) == std::get<Indices>(that)) || ...);
            }
            (std::make_index_sequence<sizeof...(Iterators)>{});
        }

        template <std::size_t I>
        constexpr auto &&get() &noexcept(noexcept(std::get<I>(static_cast<base &>(*this)))) {
            using namespace configuration;
            auto &self = static_cast<base &>(*this);
            if constexpr (utils::mask_element<I, Mask> && contains<Config, zip_config::MOVE_FROM_RVALUES>) {
                return std::move(*std::get<I>(self));
            } else {
                return *std::get<I>(self);
            }
        }

        template <std::size_t I>
        constexpr auto const &get() const &noexcept(noexcept(std::get<I>(static_cast<base const &>(*this)))) {
            auto self = static_cast<base const &>(*this);
            return *std::get<I>(self);
        }

        template <std::size_t I>
        constexpr auto get() &&noexcept(noexcept(std::get<I>(static_cast<base &&>(*this)))) {
            using namespace configuration;
            auto &&self = static_cast<base &&>(*this);
            if constexpr (utils::mask_element<I, Mask> && contains<Config, zip_config::MOVE_FROM_RVALUES>) {
                return std::move(*std::get<I>(self));
            } else {
                return *std::get<I>(self);
            }
        }
    };


    template <configuration::zip_config Config, typename Mask, std::forward_iterator... Iterators>
    class zip_iterator : private zip_value<Config, Mask, Iterators...> {
       private:
        using base = zip_value<Config, Mask, Iterators...>;

       public:
        explicit constexpr zip_iterator(Iterators &&...iterators) noexcept(
            noexcept(base(std::forward<Iterators>(iterators)...)))
            : base(std::forward<Iterators>(iterators)...) {}

        constexpr zip_iterator() noexcept(noexcept(base())) = default;

        using value_type = base;
        using reference = value_type &;
        using const_reference = value_type const &;
        using difference_type = int;
        using iterator_category = std::forward_iterator_tag;

        constexpr reference operator*() const noexcept {
            return const_cast<reference>(static_cast<const_reference>(*this));
        }

        constexpr zip_iterator &operator++() noexcept(noexcept(this->increment())) {
            this->increment();
            return *this;
        }

        constexpr zip_iterator operator++(int) &noexcept(
            std::is_nothrow_copy_constructible_v<zip_iterator> &&noexcept(this->increment())) {
            auto copy = *this;
            this->increment();
            return copy;
        }

        constexpr bool operator==(zip_iterator const &other) const noexcept(noexcept(this->equals(other))) {
            return this->equals(other);
        }

        constexpr bool operator!=(zip_iterator const &other) const noexcept(noexcept(this->equals(other))) {
            return !this->equals(other);
        }
    };


    template <configuration::zip_config Config, typename Mask, std::forward_iterator... Iterators>
    constexpr auto make_zip_iterator(Iterators &&...iterators) noexcept(
        noexcept(zip_iterator<Config, Mask, Iterators...>(std::forward<Iterators>(iterators)...))) {
        return zip_iterator<Config, Mask, Iterators...>(std::forward<Iterators>(iterators)...);
    }


    template <configuration::zip_config Config, std::ranges::forward_range... Containers>
    class zip_impl : private std::tuple<Containers...> {
        using base = std::tuple<Containers...>;
        using mask = utils::types_to_rvalues_mask<Containers &&...>;

        static_assert(sizeof...(Containers) > 0, "At least one container required.");

       public:
        using std::tuple<Containers...>::tuple;

        constexpr auto begin() noexcept(noexcept(
            make_zip_iterator<Config, mask>(std::begin(std::get<Containers>(static_cast<base &>(*this)))...))) {
            return [&]<std::size_t... Indices>(std::index_sequence<Indices...>) {
                return make_zip_iterator<Config, mask>(std::begin(std::get<Indices>(static_cast<base &>(*this)))...);
            }
            (std::make_index_sequence<sizeof...(Containers)>{});
        }

        constexpr auto end() noexcept(
            noexcept(make_zip_iterator<Config, mask>(std::end(std::get<Containers>(static_cast<base &>(*this)))...))) {
            return [&]<std::size_t... Indices>(std::index_sequence<Indices...>) {
                return make_zip_iterator<Config, mask>(std::end(std::get<Indices>(static_cast<base &>(*this)))...);
            }
            (std::make_index_sequence<sizeof...(Containers)>{});
        }
    };

}  // namespace zip_utils::detail::impl


namespace std {

    template <zip_utils::configuration::zip_config Config, typename Mask, typename... Types>
    struct tuple_size<zip_utils::detail::impl::zip_value<Config, Mask, Types...>> {
        static constexpr std::size_t value = sizeof...(Types);
    };

    template <zip_utils::configuration::zip_config Config, std::size_t I, typename Mask, typename... Types>
    struct tuple_element<I, zip_utils::detail::impl::zip_value<Config, Mask, Types...>> {
        using iterator = zip_utils::detail::utils::get_at<I, Types...>;
        using type = std::remove_reference_t<typename iterator_traits<iterator>::reference>;
    };

    template <zip_utils::configuration::zip_config Config, std::size_t I, typename Mask, typename... Types>
    struct tuple_element<I, zip_utils::detail::impl::zip_value<Config, Mask, Types...> const> {
        using type = const tuple_element_t<I, zip_utils::detail::impl::zip_value<Config, Mask, Types...>>;
    };

}  // namespace std
