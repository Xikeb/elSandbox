#include "el/integral_c.hpp"

namespace el {
	template<typename T, typename U>
	constexpr el::false_c is_same_game(T t, U u) {
		return { };
	}
	
	template<typename T>
	constexpr el::true_c is_same_game(T t, T u) {
		return { };
	}
} // el