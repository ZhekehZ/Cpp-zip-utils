#pragma once

#include <tuple>
#include <type_traits>

namespace zip_utils::detail::parameter_pack_utils {

    template<std::size_t Size, template<typename> typename Criteria, typename... Ts>
    struct filter_impl;

    template<std::size_t...>
    struct Indices { };

    template<std::size_t Size, template<typename> typename Criteria>
    struct filter_impl<Size, Criteria> {
        using indices = Indices<>;
        using type = std::tuple<>;
    };

    template<typename T, typename NextTuple>
    struct tuple_append {
        template<typename... Ts>
        static std::tuple<T, Ts...>* impl(std::tuple<Ts...>*) {
            return nullptr;
        }

        using type = std::decay_t<decltype(*impl(static_cast<NextTuple*>(nullptr)))>;
    };

    template<std::size_t I, typename NextSeq>
    struct integer_sequence_append {
        template<std::size_t... Is>
        static Indices<I, Is...>* impl(Indices<Is...>*) {
            return nullptr;
        }

        using indices = std::decay_t<decltype(*impl(static_cast<NextSeq*>(nullptr)))>;
    };

    template<std::size_t Size,
             template<typename> typename Criteria,
             typename T, typename... Ts>
    struct filter_impl<Size, Criteria, T, Ts...> {
        static constexpr bool is_ok = Criteria<T>::value;
        static constexpr std::size_t index = Size - sizeof...(Ts) - 1;

        using next = filter_impl<Size, Criteria, Ts...>;

        using indices = std::conditional_t<filter_impl::is_ok,
                                           typename integer_sequence_append<filter_impl::index,
                                                                            typename next::indices>::indices,
                                           typename next::indices>;
        using type = std::conditional_t<filter_impl::is_ok,
                                        typename tuple_append<T, typename next::type>::type,
                                        typename next::type>;
    };

    template<template<typename> typename Func, typename Tuple>
    struct map_impl {
        template<typename... Ts>
        static std::tuple<typename Func<Ts>::type...>* impl(std::tuple<Ts...>*) {
            return nullptr;
        }

        using type = std::decay_t<decltype(*impl(static_cast<Tuple*>(nullptr)))>;
    };

    template<template<typename> typename Criteria, typename... Ts>
    using filter = typename filter_impl<sizeof...(Ts), Criteria, Ts...>::type;

    template<template<typename> typename Criteria, typename... Ts>
    using filter_indices = typename filter_impl<sizeof...(Ts), Criteria, Ts...>::indices;

    template<template<typename> typename Func, typename Tuple>
    using map = typename map_impl<Func, Tuple>::type;

    template<template<typename> typename Criteria, typename... Args>
    bool constexpr at_least_one_is = (Criteria<Args>::value || ... || false);

    template<std::size_t Idx, std::size_t... Indices>
    constexpr std::size_t count_less_than = ((Indices < Idx ? 1 : 0) + ... + 0);

    template<std::size_t Idx, std::size_t... Indices>
    constexpr std::size_t is_in = ((Idx == Indices) || ... || false);

    template<template<typename> typename Constructor>
    struct wrap {
        template<typename T>
        struct impl {
            using type = Constructor<T>;
        };
    };

}// namespace zip_utils::detail::parameter_pack_utils