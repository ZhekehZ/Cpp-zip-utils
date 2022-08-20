#pragma once

#include <cstddef>


namespace zip_utils::configuration {

    enum class zip_config : std::size_t {
        NONE = 0b0,
        MOVE_FROM_RVALUES = 0b1,  // Elements of rvalue containers will be moved
    };


    template <zip_config Lhs, zip_config Rhs>
    static constexpr bool contains = (static_cast<std::size_t>(Lhs) & static_cast<std::size_t>(Rhs)) != 0;


    inline consteval zip_config operator|(zip_config lhs, zip_config rhs) {
        return static_cast<zip_config>(static_cast<std::size_t>(lhs) | static_cast<std::size_t>(rhs));
    }

}  // namespace zip_utils::configuration
