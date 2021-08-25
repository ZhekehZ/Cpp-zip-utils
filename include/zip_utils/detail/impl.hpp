#pragma once

#include "parameter_pack_utils.hpp"
#include "utils.hpp"
#include <concepts>
#include <type_traits>

namespace zip_utils::detail::impl {

	template<std::forward_iterator... Iterators>
	class zip_iterator {
	public:
		using value_type = utils::iterator_pack<Iterators...>;
		using reference = value_type&;
		using iterator_category = std::forward_iterator_tag;
		using difference_type = int;
		using IterPack = utils::iterator_pack<Iterators...>;

		constexpr zip_iterator() noexcept = default;

		constexpr explicit zip_iterator(Iterators... iterators) noexcept(noexcept(IterPack(iterators...)))
			: iterator(iterators...) {
		}

		constexpr reference operator*() const noexcept {
			return const_cast<reference>(iterator);
		}

		constexpr zip_iterator& operator++() {
			iterator.inc();
			return *this;
		}

		constexpr zip_iterator operator++(int) noexcept(noexcept(zip_iterator(++*this))) {
			auto copy = *this;
			++*this;
			return copy;
		}

		constexpr bool operator==(zip_iterator const& other) const
			noexcept(noexcept(iterator == iterator)) {
			return iterator == other.iterator;
		}

		constexpr bool operator!=(zip_iterator const& other) const
			noexcept(noexcept(iterator == iterator)) {
			return iterator != other.iterator;
		}

		template<std::size_t I>
		constexpr void next(std::size_t val) noexcept(noexcept(this->iterator.template next<I>(val))) {
			iterator.template next<I>(val);
		}

	private:
		IterPack iterator;
	};

	static_assert(std::forward_iterator<zip_iterator<int*>>);

	template<std::forward_iterator... Iterators>
	class zip_impl {
		static_assert(sizeof...(Iterators) > 0, "AT_LEAST_ONE_ARGUMENT_NEEDED");
		using Iter = zip_iterator<Iterators...>;

	public:
		constexpr zip_impl(Iter&& begin, Iter&& end) noexcept(std::is_nothrow_move_constructible_v<Iter>)
			: begin_(std::move(begin)), end_(std::move(end)) {
		}

		constexpr auto begin() const noexcept(std::is_nothrow_copy_constructible_v<Iter>) {
			return begin_;
		}

		constexpr auto end() const noexcept(std::is_nothrow_copy_constructible_v<Iter>) {
			return end_;
		}

	private:
		Iter begin_;
		Iter end_;
	};

	template<typename Storage, std::forward_iterator... Iterators>
	class zip_impl_with_rvalue_collections {
		static_assert(sizeof...(Iterators) > 0, "AT_LEAST_ONE_ARGUMENT_NEEDED");
		using Iter = zip_iterator<Iterators...>;

	public:
		template<size_t... RvalueIndices, typename... Containers, size_t... Indices>
		constexpr zip_impl_with_rvalue_collections(
			parameter_pack_utils::Indices<RvalueIndices...>,
			std::integer_sequence<size_t, Indices...>,
			Containers&&... containers)
			: storage_(), begin_(zip_iterator{
							  std::begin(
								  utils::save_to_storage<Indices, Storage, Containers, RvalueIndices...>(
									  storage_, std::forward<Containers>(containers)))...}),
			  end_(zip_iterator{
				  std::end(
					  utils::get_from_storage<Indices, Storage, Containers, RvalueIndices...>(
						  storage_, std::forward<Containers>(containers)))...}) {
		}

		constexpr auto begin() const noexcept(std::is_nothrow_copy_constructible_v<Iter>) {
			return begin_;
		}

		constexpr auto end() const noexcept(std::is_nothrow_copy_constructible_v<Iter>) {
			return end_;
		}

	private:
		Storage storage_;
		Iter begin_;
		Iter end_;
	};

	template<typename... Containers>
	constexpr auto make_zip_impl(Containers&&... containers) {
		return zip_impl{zip_iterator{std::begin(containers)...},
						zip_iterator{std::end(containers)...}};
	}

	template<typename... Containers>
	constexpr auto make_zip_impl_with_rvalue_collections(Containers&&... containers) {
		using namespace parameter_pack_utils;
		using Rvalues = map<wrap<std::optional>::impl, map<std::decay, filter<std::is_rvalue_reference, Containers&&...>>>;
		using RvalueIndices = filter_indices<std::is_rvalue_reference, Containers&&...>;

		return zip_impl_with_rvalue_collections<
			Rvalues,
			std::decay_t<decltype(std::begin(containers))>...>(
			RvalueIndices{},
			std::make_integer_sequence<size_t, sizeof...(Containers)>{},
			std::forward<Containers>(containers)...);
	}

}// namespace zip_utils::detail::impl

namespace std {

	template<typename... Iterators>
	struct tuple_size<zip_utils::detail::utils::iterator_pack<Iterators...>> {
		static constexpr size_t value = sizeof...(Iterators);
	};

	template<std::size_t I, std::forward_iterator... Iterators>
	struct tuple_element<I, zip_utils::detail::utils::iterator_pack<Iterators...>> {
		using iterator = tuple_element_t<I, tuple<Iterators...>>;
		using reference = typename iterator_traits<iterator>::reference;
		using type = std::remove_reference_t<reference>;
	};

	template<std::size_t I, std::forward_iterator... Iterators>
	struct tuple_element<I, const zip_utils::detail::utils::iterator_pack<Iterators...>> {
		using type = const tuple_element_t<I, zip_utils::detail::utils::iterator_pack<Iterators...>>;
	};

}// namespace std