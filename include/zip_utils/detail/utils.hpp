#pragma once

#include "parameter_pack_utils.hpp"
#include <iterator>
#include <ranges>
#include <tuple>
#include <utility>

namespace zip_utils::detail::utils {

    template<typename Tuple, std::size_t I>
    static constexpr bool nothrow_deref_tuple_element = requires {
        requires requires(std::tuple_element_t<I, Tuple> & t) {
            { *t }
            noexcept;
        };
    };

    template<typename T>
    static constexpr bool nothrow_comparable = requires(const T& t) {
        { t == t }
        noexcept;
    };

    template<typename T>
    static constexpr bool nothrow_incrementable = requires(T& t) {
        { ++t }
        noexcept;
    };

    template<std::forward_iterator... Iterators>
    class iterator_pack : public std::tuple<Iterators...> {
    public:
        using base = std::tuple<Iterators...>;
        using base::tuple;

        template<std::size_t I>
        constexpr auto& get() & noexcept(nothrow_deref_tuple_element<base, I>) {
            return *std::get<I>(static_cast<base>(*this));
        }

        template<std::size_t I>
        [[nodiscard]] constexpr const auto& get() const& noexcept(nothrow_deref_tuple_element<const base, I>) {
            const auto& self = static_cast<const base&>(*this);
            return *std::get<I>(self);
        }

        template<std::size_t I>
        constexpr auto get() && noexcept(nothrow_deref_tuple_element<base, I>) {
            auto& self = static_cast<base&>(*this);
            return *std::get<I>(self);
        }

        template<std::size_t I>
        [[nodiscard]] constexpr auto get() const&& noexcept(nothrow_deref_tuple_element<const base, I>) {
            const auto& self = static_cast<const base&>(*this);
            return *std::get<I>(self);
        }

        constexpr bool operator==(iterator_pack const& other) const
            noexcept((nothrow_comparable<Iterators> && ...)) {
            const base& self = *this;
            const base& that = other;
            return [&]<std::size_t... I>(std::index_sequence<I...>) {
                return ((std::get<I>(self) == std::get<I>(that)) || ...);
            }
            (std::make_index_sequence<sizeof...(Iterators)>{});
        }

        constexpr bool operator!=(iterator_pack const& other) const
            noexcept((nothrow_comparable<Iterators> && ...)) {
            return !(*this == other);
        }

        template<std::size_t I>
        constexpr void next(std::size_t val)
        noexcept(requires(base& ip) {{ ip = std::next(std::get<I>(ip), val) } noexcept; }) {
            base& self = *this;
            auto& it = std::get<I>(self);
            it = std::next(it, val);
        }

        constexpr void inc() noexcept((nothrow_incrementable<Iterators> && ...)) {
            auto inc = [&]<std::size_t... I>(std::index_sequence<I...>, base & tup) {
                (++std::get<I>(tup), ...);
            };

            // Strong exception guarantee
            if constexpr ((nothrow_incrementable<Iterators> && ...)) {
                inc(std::make_index_sequence<sizeof...(Iterators)>{}, *this);
            }
            else {
                base self = *this;
                inc(std::make_index_sequence<sizeof...(Iterators)>{}, self);
                static_cast<base&>(*this) = self;
            }
        }
    };

    template<typename T>
    static constexpr bool is_nothrow_beg_end_copy_constructible_v = requires(T&& arg) {
        requires noexcept(std::begin(std::forward<T>(arg)));
        requires noexcept(std::end(std::forward<T>(arg)));
        requires std::is_nothrow_copy_constructible_v<
            std::remove_cvref_t<decltype(std::begin(arg))>>;
    };

    template<std::size_t CurrIdx, typename StorageT, typename Container, std::size_t... IndicesToSave>
    constexpr Container& save_to_storage(
        StorageT& storage,
        Container&& container) {
        if constexpr (parameter_pack_utils::is_in<CurrIdx, IndicesToSave...>) {
            constexpr std::size_t IndexToSave = parameter_pack_utils::count_less_than<CurrIdx, IndicesToSave...>;
            get<IndexToSave>(storage) = std::forward<Container>(container);
            return get<IndexToSave>(storage).value();
        }
        else {
            return std::forward<Container>(container);
        }
    }

    template<std::size_t CurrIdx, typename StorageT, typename Container, std::size_t... IndicesToSave>
    constexpr Container& get_from_storage(
        StorageT& storage,
        Container&& container) {
        if constexpr (parameter_pack_utils::is_in<CurrIdx, IndicesToSave...>) {
            constexpr std::size_t IndexToSave = parameter_pack_utils::count_less_than<CurrIdx, IndicesToSave...>;
            return get<IndexToSave>(storage).value();
        }
        else {
            return std::forward<Container>(container);
        }
    }

}// namespace zip_utils::detail::utils