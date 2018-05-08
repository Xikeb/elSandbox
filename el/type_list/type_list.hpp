/*
 * type_list.h
 *
 *  Created on: 22 nov. 2017
 *      Author: eliord
 */

#ifndef TYPE_LIST_H_
	#define TYPE_LIST_H_

	#include <cstdlib>
	#include "el/detail/exists.hpp"
	#include "el/types/nothing.hpp"
	#include "el/types/tag.hpp"
	#include "el/types/integral_c.hpp"
	#include "el/types/at.hpp"
	#include "el/types/void.hpp"
	#include "el/conditional.hpp"
	#include "el/enable_if.hpp"
	#include "el/is_same.hpp"

	namespace el {
		template <typename ...Types>
		struct type_list;

		template<typename THead, typename ...TRest>
		struct type_list<THead, TRest...> {
			using List = el::type_list<THead, TRest...>;
			using Current = THead;
			using Next = type_list<TRest...>;
			
			constexpr type_list() { }

			constexpr static std::size_t size = 1 + sizeof...(TRest);

			template<typename TElem>
			using Contains = typename el::conditional<
				el::is_same<Current, TElem>::value,
				el::true_c,
				typename Next::template Contains<TElem>
			>::type;

			template<typename TElem, std::size_t TPos = 0>
			using IndexOf = typename el::conditional<
				el::is_same<Current, TElem>::value,
				el::size_c<TPos>,
				typename Next::template IndexOf<TElem, TPos + 1>
			>::type;
		};

		template<>
		struct type_list<> {
			using List = el::type_list<>;

			constexpr type_list() { }

			constexpr static std::size_t size = 0;

			template<typename TElem>
			using Contains = el::false_c;

			template<typename TElem, std::size_t TPos = 0>
			using IndexOf = el::false_c;
		};

		namespace impl {
			//If second overload is dropped, this one is used
			template<typename T, typename = void>
			struct IsEnd {
				using Result = el::true_c;
			};

			/*
			 * If has member, this overload is chosen (even though
			 * second template argument still evaluates to type void)
			 * Else, second argument is SFN=INAE'd away, first overload
			 * which offers a default value for second argument is chosen.
			*/
			template<typename T>
			struct IsEnd<T, el::void_t<typename T::Current>> {
				using Result = el::false_c;
			};
		} // impl

		template<typename List>
		using IsEnd = typename el::impl::IsEnd<List>::Result;

		/*
		 * At
		 */
		namespace impl {
			template<std::size_t pos, typename List>
			struct at_helper;

			template<typename List>
			struct at_helper<0, List> {
				using Type = typename List::Current;
			};

			template<typename std::size_t pos, typename List>
			struct at_helper {
				using Type = el::enable_if<
					el::detail::exists<typename List::Next>::value,
					el::impl::at_helper<pos - 1, typename List::Next>
				>;
			};
		} // impl
		template<std::size_t pos, typename List>
		struct at {
			using Type = el::impl::at_helper<pos, List>;
		};

		/*
		 * Tag
		 */
		struct type_list_tag {
		};

		template<typename ...Types>
		struct tag<el::type_list<Types...>> {
			using Type = type_list_tag;
		};

		/*
		 * Rename
		 */
		namespace impl {
			template<template<typename...> class Apply, typename List>
			struct Rename;

			template<template<typename...> class Apply, typename ...Types>
			struct Rename<Apply, el::type_list<Types...>> {
				using Type = Apply<Types...>;
			};
		} // impl
		template<template<typename...> class Apply, typename ...Types>
		using Rename = typename el::impl::Rename<Apply, Types...>::Type;
	} //namespace el
#endif /* TYPE_LIST_H_ */
