#ifndef ELMETA_EXISTS_HPP
	#define ELMETA_EXISTS_HPP
	#include "el/types/integral_c.hpp"

	namespace el {
		namespace impl {
			template<typename T>
			constexpr int exists(int = 0) noexcept;

			constexpr void exists(...);
		} // impl
		namespace detail {
			// template<typename ...Types>
			// using exists = el::bool_c<sizeof...(Types) != 0>;

			template<typename T>
			using exists = el::bool_c<sizeof(el::impl::exists<T>()) != 0>;
		} // detail
	} // el
#endif // ELMETA_EXISTS_HPP