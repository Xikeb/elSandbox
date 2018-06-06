#ifndef EL_ENABLE_IF_H
	#define EL_ENABLE_IF_H

	namespace el {
		template<bool v, typename T>
		struct enable_if {
		};

		template<typename T>
		struct enable_if<true, T> {
			using type = T;
		};

		template<bool v, typename T>
		using enable_if_t = typename el::enable_if<v,T>::type;
	} // el
#endif // EL_ENABLE_IF_H