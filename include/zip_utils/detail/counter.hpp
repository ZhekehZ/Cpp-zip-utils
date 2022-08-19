#pragma once

#include <iterator>

#include "utils.hpp"

namespace zip_utils::detail::counter {

    class counting_iterator {
       public:
        constexpr counting_iterator() : index_(0){};

        using value_type = const std::size_t;
        using reference = value_type &;
        using iterator_category = std::forward_iterator_tag;
        using difference_type = int;


        constexpr reference operator*() const noexcept {
            return index_;
        }


        constexpr counting_iterator &operator++() noexcept {
            ++index_;
            return *this;
        }


        constexpr counting_iterator operator++(int) &noexcept {
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


    inline constexpr counting_iterator end_count{};


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

}  // namespace zip_utils::detail::counter
