#ifndef ELMETA_TYPE_LIST_H_
	#define ELMETA_TYPE_LIST_H_
	#include <cstdlib>
	#include <utility>
	#include "el/detail/exists.hpp"
	#include "el/types/nothing.hpp"
	#include "el/types/tag.hpp"
	#include "el/types/integral_c.hpp"
	#include "el/types/type_c.hpp"
	#include "el/types/void.hpp"
	#include "el/conditional.hpp"
	#include "el/enable_if.hpp"
	#include "el/is_same.hpp"
	#include "el/type_list/filter.hpp"
	#include "el/type_list/for_each.hpp"
	#include "el/type_list/every.hpp"
	#include "el/type_list/reduce.hpp"
	#include "el/type_list/some.hpp"
	#include "el/type_list/contains.hpp"
	#include "el/type_list/index_of.hpp"
	#include "el/type_list/at.hpp"

	namespace el {
		template <typename ...Types>
		struct type_list_t;

		//name this to 'type_list_t' and make a default constexpr static instance of the class 
		template<>
		struct type_list_t<> {
			using Self = el::type_list_t<>;

			constexpr static std::size_t size = 0;

			template<typename>
			using Contains = el::false_c;

//			struct Has {
//				template<typename T>
//				constexpr auto operator()() const noexcept { return el::false_c{}; }
//
//				template<typename T>
//				constexpr auto operator()(el::type_t<T>) const noexcept { return el::false_c{}; }
//			};
//
//			constexpr static Has has{};

			template<typename, std::size_t = 0>
			using IndexOf = el::false_c;

			template<typename = void>
			using Filter = el::type_list_t<>;

			using Shift = el::type_list_t<>;
			template<typename ...Ts>
			using Unshift = el::type_list_t<Ts...>;
			template<typename ...Ts>
			using Push = el::type_list_t<Ts...>;

			template<typename ...Ts>
			constexpr static auto push() noexcept { return el::type_list_t<Ts...>{}; }
			template<typename ...Ts>
			constexpr static auto push(el::type_t<Ts>...) noexcept { return el::type_list_t<Ts...>{}; }
			template<typename ...Ts>
			constexpr static auto unshift() noexcept { return el::type_list_t<Ts...>{}; }
			template<typename ...Ts>
			constexpr static auto unshift(el::type_t<Ts>...) noexcept { return el::type_list_t<Ts...>{}; }

			template<std::size_t N = 1>
			constexpr static auto pop(el::size_c<N> = {}) noexcept { return el::type_list_t<>{}; }
			template<std::size_t N = 1>
			constexpr static auto shift(el::size_c<N> = {}) noexcept { return el::type_list_t<>{}; }

			template<typename T>
			constexpr static auto contains(el::type_t<T> = {}) noexcept { return el::impl::contains<T>(el::type_c<Self>, 0); }
			template<typename T>
			constexpr static auto index_of(el::type_t<T> = {}) noexcept { return el::impl::index_of<T>(el::size_c<0>{}, el::type_c<Self>, 0); }

			template<typename TF, typename ...Args>
			constexpr static auto for_each(TF&& f, Args&&...) noexcept { return std::move(std::forward<TF>(f)); }

			template<typename TF, typename ...Args>
			constexpr static auto every(TF&&, Args&&...) noexcept { return el::false_c{}; }

			template<typename TF, typename ...Args>
			constexpr static auto some(TF&&, Args&&...) noexcept { return el::true_c{}; }

			template<typename TF, typename Acc, typename ...Args>
			constexpr static auto reduce(TF&&, Acc&& acc, Args&&...) { return std::forward<Acc>(acc); }
		};

		template<typename THead, typename ...TRest>
		struct type_list_t<THead, TRest...> {
			using Self = el::type_list_t<THead, TRest...>;
			using Current = THead;
			using Next = type_list_t<TRest...>;
			using First = THead;

			constexpr static std::size_t size = 1 + sizeof...(TRest);

			template<typename T>
			struct Contains: TYPE_OF(el::impl::contains<T>(el::type_c<Self>, 0)) {
			};

			template<typename TElem, std::size_t TPos = 0>
			using IndexOf = typename el::conditional_t<
				el::is_same_v<Current, TElem>,
				el::size_c<TPos>,
				typename Next::template IndexOf<TElem, TPos + 1>
			>;

			template<std::size_t Pos>
			using At = typename decltype(+el::impl::at(el::type_c<Self>, el::size_c<Pos>{}, 0))::type;

			using Shift = Next;
			template<typename ...T>
			using Unshift = el::type_list_t<T..., THead, TRest...>;
			template<typename ...T>
			using Push = el::type_list_t<THead, TRest..., T...>;

			template<typename ...Ts>
			constexpr static auto push() noexcept { return el::type_list_t<THead, TRest..., Ts...>{}; }
			template<typename ...Ts>
			constexpr static auto push(el::type_t<Ts>...) noexcept { return el::type_list_t<THead, TRest..., Ts...>{}; }
			template<typename ...Ts>
			constexpr static auto unshift() noexcept { return el::type_list_t<Ts..., THead, TRest...>{}; }
			template<typename ...Ts>
			constexpr static auto unshift(el::type_t<Ts>...) noexcept { return el::type_list_t<Ts..., THead, TRest...>{}; }

			template<std::size_t N = 1>
			constexpr static auto pop(el::size_c<N> = {}) noexcept { return el::type_list_t<>{}; }
			template<std::size_t N = 1>
			constexpr static auto shift(el::size_c<N> = {}) noexcept { return el::type_list_t<>{}; }

			template<typename T>
			constexpr static auto contains(el::type_t<T> = {}) noexcept { return el::impl::contains<T>(el::type_c<Self>, 0); }
			template<typename T>
			constexpr static auto index_of(el::type_t<T> = {}) noexcept { return el::impl::index_of<T>(el::size_c<0>{}, el::type_c<Self>, 0); }
			template<std::size_t Id>
			constexpr static auto at(el::size_c<Id> idx = {}) noexcept {
				static_assert(Id < Self::size, "type_list.at(): Index out of bounds");
				return el::impl::at(el::type_c<Self>, idx, 0);
			}


			template<typename Pred>
			constexpr static auto filter(Pred&& c) {
				return el::impl::filter(
					el::type_c<el::type_list_t<>>,
					el::type_c<Self>,
					std::forward<Pred>(c)
				);
			}

			/*template<typename Pred>
			using Filter = el::type_of<decltype(el::impl::filter<Pred>(
				el::type_c<el::type_list_t<>>,
				el::type_c<Self>
			))>;*/
			template<typename Pred>
			using Filter = TYPE_OF(el::impl::filter(
				el::type_c<el::type_list_t<>>,
				el::type_c<Self>,
				Pred{}
			));

			template<typename TF, typename ...Args>
			constexpr static auto for_each(TF&& f, Args&&... args) noexcept {
				return el::impl::for_each<Self>(
					el::size_c<0>(),
					std::forward<TF>(f),
					std::forward<Args>(args)...
				);
			}

			template<typename TF, typename ...Args>
			constexpr static auto every(TF&& f, Args&&... args) noexcept {
				return el::impl::every<Self>(
					el::size_c<0>(),
					std::forward<TF>(f),
					std::forward<Args>(args)...
				);
			}

			template<typename TF, typename ...Args>
			constexpr static auto some(TF&& f, Args&&... args) noexcept {
				return el::impl::some<Self>(
					el::size_c<0>(),
					std::forward<TF>(f),
					std::forward<Args>(args)...,
					int{}
				);
			}

			template<typename TF, typename Acc, typename ...Args>
			constexpr static auto reduce(TF&& f, Acc&& acc, Args&&... args) noexcept {
				return el::impl::reduce<Self>(
					el::size_c<size - 1>(),
					std::forward<TF>(f),
					std::forward<Acc>(acc),
					std::forward<Args>(args)...,
					int{}
				);
			}

			template <std::size_t Id>
			constexpr auto operator[](el::size_c<Id> idx) const noexcept { return this->at(idx); }
		};

		template<typename ...Types>
		constexpr static auto type_list = el::type_list_t<Types...>{};

		namespace impl {
			template<typename T>
			struct IsTypeList: el::false_c {};
			template<typename ...Types>
			struct IsTypeList<el::type_list_t<Types...>>: el::true_c {};
		} // impl

		template<typename T>
		using is_type_list = el::impl::IsTypeList<T>;
		/*
		 * At
		 */
//		namespace impl {
//			template<typename std::size_t pos, typename List>
//			struct at_helper {
//				using Type = typename el::enable_if<
//					el::detail::exists<typename List::Next>::value,
//					typename el::impl::at_helper<pos - 1, typename List::Next>::Type
//				>::type;
//			};
//
//			template<typename List>
//			struct at_helper<0, List> {
//				using Type = typename List::Current;
//			};
//		} // impl

		/*
		 * Tag
		 */
		struct type_list_tag {
		};

		template<typename ...Types>
		struct tag<el::type_list_t<Types...>> {
			using Type = type_list_tag;
		};

		/*
		 * Rename
		 */
		namespace impl {
			template<template<typename...> class Apply, typename List>
			struct Rename;

			template<template<typename...> class Apply, typename ...Types>
			struct Rename<Apply, el::type_list_t<Types...>> {
				using Type = Apply<Types...>;
			};
		} // impl
		template<template<typename...> class Apply, typename ...Types>
		using Rename = typename el::impl::Rename<Apply, Types...>::Type;
	} //namespace el
#endif /* ELMETA_TYPE_LIST_H_ */
