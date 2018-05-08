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
			template<bool a, bool b, bool ...rest>
			using _and = typename el::impl::_and<a, b, rest...>::Result;
		} // detail
	} // el
#endif // ELMETA_AND_HPP