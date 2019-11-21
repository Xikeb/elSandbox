#ifndef ELMETA_IS_SAME_H_
# define ELMETA_IS_SAME_H_

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

	    template <template <typename ...> class T, typename U>
	    struct is_similar {
	        using Result = el::false_c;
	    };

	    template <template <typename ...> class T, typename ...Args>
	    struct is_similar<T, T<Args...>> {
	        using Result = el::true_c;
	    };
	} // impl

	template<typename T, typename U>
	using is_same = typename el::impl::is_same<T, U>::Result;

	template<typename T, typename U>
	constexpr bool is_same_v = el::is_same<T, U>::value;

	template<template<typename ...> class T, typename U>
	using is_similar = typename el::impl::is_similar<T, U>::Result;

	template<template<typename ...> class T, typename U>
	constexpr bool is_similar_v = el::is_similar<T, U>::value;
}
#endif /*ELMETA_IS_SAME_H_*/