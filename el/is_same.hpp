/*
 * type_list.h
 *
 *  Created on: 22 nov. 2017
 *      Author: eliord
 */

#ifndef IS_SAME_H_
# define IS_SAME_H_

# include "el/types/integral_c.hpp"

namespace el {
	namespace impl {
	    template <typename T, typename U>
	    struct is_same {
	        using Result = el::false_c;
	    };

	    template <typename T>
	    struct is_same<T, T> {
	        using Result = el::true_c;
	    };
	} // impl

	template<typename T, typename U>
	using is_same = typename el::impl::is_same<T, U>::Result;

	template<typename T, typename U>
	constexpr bool is_same_v = el::is_same<T, U>::value;
}
#endif /*IS_SAME_H_*/