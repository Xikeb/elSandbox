#ifndef ELMETA_AND_HPP
	#define ELMETA_AND_HPP
	#include <vector>
	#include "el/types/integral_c.hpp"

	namespace el {
		namespace impl {
			template<bool a, bool b, bool ...rest>
			struct _and;

			template<>
			struct _and<true, true> {
				using Result = el::true_c;
			};

			template<bool a, bool ...rest>
			struct _and<false, a, rest...> {
				using Result = el::false_c;
			};

			template<bool a, bool ...rest>
			struct _and<a, false, rest...> {
				using Result = el::false_c;
			};

			template<bool ...rest>
			struct _and<true, true, rest...> {
				using Result = typename el::impl::_and<true, rest...>::Result;
			};
		} // impl

		namespace detail {
			template<bool ...values>
			using _and = typename el::impl::_and<values...>::Result;

			constexpr auto andf() {
				return el::true_c{};
			}

			template<bool ...Values>
			constexpr auto andf(el::integral_c<bool, true>, el::integral_c<bool, Values>... rest) {
				return el::detail::andf(rest...);
			}

			template<bool ...Values>
			constexpr auto andf(el::integral_c<bool, false>, el::integral_c<bool, Values>...) {
				return el::false_c{};
			}
		} // detail
	} // el
#endif // ELMETA_AND_HPP