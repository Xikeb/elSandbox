#ifndef EL_REMOVE_VOLATILE_H
#define EL_REMOVE_VOLATILE_H

namespace el {
	template<typename T> struct remove_volatile;

	template<typename T>
	using remove_volatile_t = typename remove_volatile<T>::type;

	template<typename T>
	struct remove_volatile {
		using type = T;
	};

	template<typename T>
	struct remove_volatile<volatile T> {
		using type = T;
	};
} // el

#endif // EL_REMOVE_VOLATILE_H
