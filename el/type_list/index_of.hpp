#ifndef ELMETA_INDEXOF_HPP
	#define ELMETA_INDEXOF_HPP
	#include "el/types/integral_c.hpp"
	#include "el/types/type_c.hpp"

	namespace el {
		namespace impl {
			template<typename T, std::size_t Idx, typename ...TRest>
			constexpr auto index_of(el::size_c<Idx> idx, el::type_t<el::type_list_t<T, TRest...>>, int = 0) noexcept {
				return idx;
			}

			template<typename T, std::size_t Idx>
			constexpr auto index_of(el::size_c<Idx>, el::type_t<el::type_list_t<>>, int = 0) noexcept {
				return el::false_c{};
			}

			template<typename T, std::size_t Idx, typename THead, typename ...TRest>
			constexpr auto index_of(el::size_c<Idx>, el::type_t<el::type_list_t<THead, TRest...>>, ...) noexcept {
				return index_of<T>(el::size_c<Idx + 1>{}, el::type_c<el::type_list_t<TRest...>>, 0);
			}
		} // impl
	} // el
#endif // ELMETA_INDEXOF_HPP