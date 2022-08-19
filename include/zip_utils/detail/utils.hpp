#pragma once

#include <cstddef>
#include <type_traits>


namespace zip_utils::detail::utils {

    template <bool... Values>
    struct bool_mask {};

    template <std::size_t I, bool Value, bool... Values>
    constexpr bool mask_element_impl(bool_mask<Value, Values...>) {
        if constexpr (I == 0) {
            return Value;
        } else {
            return mask_element_impl<I - 1, Values...>({});
        }
    }

    template <std::size_t I, typename Mask>
    constexpr bool mask_element = mask_element_impl<I>(Mask{});

    template <typename... Types>
    using types_to_rvalues_mask = bool_mask<std::is_rvalue_reference_v<Types>...>;

    template <typename... Types>
    consteval bool all_are_lvalues_or_nothrow_movable() {
        return ((std::is_lvalue_reference_v<Types> || std::is_nothrow_move_constructible_v<std::decay_t<Types>>)&&...);
    }

    template <typename T>
    struct remove_rvalue_ref {
        using type = std::remove_cvref_t<T>;
    };

    template <typename T>
    struct remove_rvalue_ref<T &> {
        using type = T &;
    };

    template <typename T>
    using remove_rvalue_ref_t = typename remove_rvalue_ref<T>::type;

    static_assert(std::is_same_v<int, remove_rvalue_ref_t<int>>);
    static_assert(std::is_same_v<int, remove_rvalue_ref_t<const int>>);
    static_assert(std::is_same_v<int &, remove_rvalue_ref_t<int &>>);
    static_assert(std::is_same_v<const int &, remove_rvalue_ref_t<const int &>>);
    static_assert(std::is_same_v<int, remove_rvalue_ref_t<const int &&>>);
    static_assert(std::is_same_v<int, remove_rvalue_ref_t<int &&>>);

    template <std::size_t I, typename... Types>
    using get_at = std::tuple_element_t<I, std::tuple<Types...>>;

}  // namespace zip_utils::detail::utils
