#ifndef ELMETA_AT_HPP
	#define ELMETA_AT_HPP
	#include "el/types/integral_c.hpp"
	#include "el/types/type_c.hpp"

	namespace el {
		namespace impl {
			template<typename THead, typename ...TRest>
			constexpr auto at(el::Type_c<el::type_list_t<THead, TRest...>>, el::size_c<0>, int = 0) noexcept {
				return el::type_c<THead>;
			}

			template<std::size_t Idx>
			constexpr auto at(el::Type_c<el::type_list_t<>>, el::size_c<Idx>, int = 0) noexcept {
				return nullptr;
			}

			template<std::size_t Idx, typename THead, typename ...TRest>
			constexpr auto at(el::Type_c<el::type_list_t<THead, TRest...>>, el::size_c<Idx>, ...) noexcept {
				return at(el::type_c<el::type_list_t<TRest...>>, el::size_c<Idx - 1>{}, 0);
			}
		} // impl
	} // el
#endif // ELMETA_AT_HPP