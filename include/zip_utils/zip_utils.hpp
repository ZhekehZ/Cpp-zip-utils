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

		// TODO Rework decomposition
		using Rvalues = map<wrap<std::optional>::impl,
							map<std::decay,
								filter<std::is_rvalue_reference, Containers&&...>>>;
		using RvalueIndicesT = filter_indices<std::is_rvalue_reference, Containers&&...>;

		if constexpr (is_empty<Rvalues>) {
			return zip_impl{ zip_iterator{ std::begin(containers)...},
							 zip_iterator{ std::end(containers)...  } };
		}
		else {
			return make_zip_impl_with_rvalue_collections<Rvalues, RvalueIndicesT>(
				std::forward<Containers>(containers)...);
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
