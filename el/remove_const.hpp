#ifndef EL_REMOVE_CONST_H
	#define EL_REMOVE_CONST_H

	namespace el {
		template<typename T>
		struct remove_const;

		template<typename T>
		using remove_const_t = typename remove_const<T>::type;

		template<typename T>
		struct remove_const {
			using type = T;
		};

		template<typename T>
		struct remove_const<const T> {
			using type = T;
		};

		template<typename T>
		struct remove_const<const T&> {
			using type = T&;
		};

		template<typename T>
		struct remove_const<const T&&> {
			using type = T&&;
		};
	} // el
#endif // EL_REMOVE_CONST_H
