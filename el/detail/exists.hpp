#ifndef ELMETA_EXISTS_HPP
	#define ELMETA_EXISTS_HPP
	#include "el/types/integral_c.hpp"

	namespace el {
		namespace detail {
			template<typename ...Types>
			using exists = el::bool_c<sizeof...(Types) != 0>;
		} // detail
	} // el
#endif // ELMETA_EXISTS_HPP