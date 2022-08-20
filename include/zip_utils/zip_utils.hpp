#pragma once

#include "detail/configuration.hpp"
#include "detail/counter.hpp"
#include "detail/impl.hpp"
#include "detail/utils.hpp"

namespace zip_utils::views {

    template <configuration::zip_config Config = configuration::zip_config::NONE,
              std::ranges::forward_range... Containers>
    constexpr auto zip(Containers &&...containers) noexcept(
        detail::utils::all_are_lvalues_or_nothrow_movable<Containers &&...>()) {
        using namespace detail::impl;
        using impl = zip_impl<Config, detail::utils::remove_rvalue_ref_t<Containers>...>;
        return impl(std::forward<Containers>(containers)...);
    }

    template <configuration::zip_config Config = configuration::zip_config::NONE,
              std::ranges::forward_range... Containers>
    constexpr auto enumerate(Containers &&...containers) noexcept(
        noexcept(zip(detail::counter::counter{}, std::forward<Containers>(containers)...))) {
        using namespace detail::counter;
        return zip<Config>(counter{}, std::forward<Containers>(containers)...);
    }

    template <typename Value, std::same_as<Value>... Values>
    constexpr auto indexed(Value &&value, Values &&...values) noexcept(
        noexcept(enumerate(std::array<std::decay_t<Value>, 1>{std::forward<Value>(value)}))) {
        return enumerate(std::array<std::decay_t<Value>, sizeof...(Values) + 1>{std::forward<Value>(value),
                                                                                std::forward<Values>(values)...});
    }

}  // namespace zip_utils::views
