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
		struct Dependencies {
			using Types = decltype(reducedDependencies<Ts...>);

			constexpr Dependencies() noexcept = default;
			constexpr Dependencies(el::Type_c<el::type_list<Ts...>>) noexcept {}
			constexpr Dependencies(Ts&&...) noexcept {}
		}; //struct Dependencies
	} // config
} //ecs