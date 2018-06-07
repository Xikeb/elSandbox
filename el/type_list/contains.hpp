#ifndef ELMETA_CONTAINS_HPP
	#define ELMETA_CONTAINS_HPP
	#include "el/types/integral_c.hpp"
	#include "el/types/type_c.hpp"

	namespace el {
		namespace impl {
			template<typename T, typename ...TRest>
			constexpr auto contains(
				el::Type_c<el::type_list<T, TRest...>> const &,
				int = 0
			) noexcept
			{
				return el::true_c{};
			}

			template<typename T>
			constexpr auto contains(
				el::Type_c<el::type_list<>> const &,
				int = 0
			) noexcept
			{
				return el::false_c{};
			}

			template<typename T, typename THead, typename ...TRest>
			constexpr auto contains(
				el::Type_c<el::type_list<THead, TRest...>> const &,
				...
			) noexcept
			{
				return contains<T>(el::type_c<el::type_list<TRest...>>, 0);
			}

			template<typename List, typename T>
			struct Contains;

			template<typename ...Types, typename T>
			struct Contains<el::type_list<Types...>, T>: type_of<decltype(el::impl::contains<T>(el::type_c<el::type_list<Types...>>, 0))> {
				using List = el::type_list<Types...>;
			};
		} // impl
	} // el
#endif // ELMETA_CONTAINS_HPP