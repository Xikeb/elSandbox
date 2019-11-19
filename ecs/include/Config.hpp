#pragma once

#include "fwd/Config.hpp"

namespace ecs {
	namespace config {
		namespace impl {
			template<typename ...Ts>
			constexpr static auto reducedDependencies = el::type_list<Ts...>::reduce([](auto deps, auto element) {
				using Element = TYPE_OF(element); 	//raw decltype() of the current element

				el::static_if(el::is_similar<Dependencies, Element>{})
					.then([](auto deps, auto element) {
						return el::type_c<
							decltype(deps)::template Push<TYPE_OF(element)::Types>
						>;
					})
				.elif(el::is_similar<el::type_list, Element>{})
					.then([](auto deps, auto element) {
						return el::type_c<el::Rename<
							decltype(deps)::template Push,
							TYPE_OF(element)
						>>;
					})
				.otherwise([](auto deps, auto element) {
					return el::type_c<decltype(deps)::template Push<TYPE_OF(element)>>;
				})(deps, element);
			}, el::type_list<>{});
		} // impl

		template<typename ...Ts>
		struct dependencies {
			using Types = decltype(reducedDependencies<Ts...>);

			constexpr dependencies() noexcept = default;
			constexpr dependencies(el::Type_c<el::type_list<Ts...>>) noexcept {}
			constexpr dependencies(Ts&&...) noexcept {}

			template<typename T>
			constexpr auto operator+(el::Type_c<T>) const noexcept {
				return el::conditional_t<
					Types::template Contains<T>::value,
					dependencies<Ts...>,
					dependencies<Ts..., T>
				>{};
			}
		}; //struct dependencies
	} // config
} //ecs