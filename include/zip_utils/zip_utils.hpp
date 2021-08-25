#pragma once

#include "detail/counter.hpp"
#include "detail/impl.hpp"
#include "detail/parameter_pack_utils.hpp"

namespace zip_utils {

	template<std::ranges::forward_range... Containers>
	constexpr auto zip(Containers&&... containers)
	noexcept((detail::utils::is_nothrow_beg_end_copy_constructible_v<Containers> && ...)) {
		using namespace detail::impl;
		using namespace detail::parameter_pack_utils;

		if constexpr (at_least_one_is<std::is_rvalue_reference, Containers&&...>) {
			return make_zip_impl_with_rvalue_collections(std::forward<Containers>(containers)...);
		}
		else {
			return make_zip_impl(std::forward<Containers>(containers)...);
		}
	}

	template<std::ranges::forward_range... Containers>
	constexpr auto enumerate(Containers&&... containers)
	noexcept(noexcept(zip(std::forward<Containers>(containers)...))) {
		detail::counter::counter counter;
		return zip(counter,
				   std::forward<Containers>(containers)...);
	}

}// namespace zip_utils
