#ifndef EL_REMOVE_REF_H
# define EL_REMOVE_REF_H

namespace el {
	template <typename T> struct remove_ref;

	template <typename T>
	using remove_ref_t = typename remove_ref<T>::type;
	
	template <typename T>
	struct remove_ref {
		using type = T;
	};

	template <typename T>
	struct remove_ref<T&> {
		using type = T;
	};

	template <typename T>
	struct remove_ref<T&&> {
		using type = T;
	};
} // el

#endif /*EL_REMOVE_REF_H*/