#ifndef ELMETA_FILTER_HPP
	#define ELMETA_FILTER_HPP
	#include "el/types/type_c.hpp"
	#include "el/fwd/type_list/type_list.hpp"

	namespace el {
		namespace impl {
			template<template<typename> class, typename ...Keep>
			constexpr auto filter(
				el::Type_c<el::type_list<Keep...>> const &result,
				el::Type_c<el::type_list<>> const &
			) noexcept
			{
				return result;
			}

			template<template<typename> class Filter, typename ...Keep, typename THead, typename ...TRest>
			constexpr auto filter(
				el::Type_c<el::type_list<Keep...>> const &,
				el::Type_c<el::type_list<THead, TRest...>> const &
			) noexcept
			{
				return el::impl::filter<Filter>(
					el::type_c<typename el::conditional<
						Filter<THead>::value,
						el::type_list<Keep..., THead>,
						el::type_list<Keep...>
					>::type>,
					el::type_c<el::type_list<TRest...>>
				);
			}
		} // impl
	} // el
#endif // ELMETA_FILTER_HPP