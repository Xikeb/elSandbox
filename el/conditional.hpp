#ifndef EL_CONDITIONAL_H
	#define EL_CONDITIONAL_H

	namespace el {
		template<bool v, typename T, typename F>
		struct conditional;

		template<bool v, typename T, typename F>
		struct conditional {
			using type = F;
		};

		template<typename T, typename F>
		struct conditional<true, T, F> {
			using type = T;
		};

		template<bool v, typename T, typename F>
		using conditional_t = typename el::conditional<v, T, F>::type;
	} // el
#endif // EL_CONDITIONAL_H
