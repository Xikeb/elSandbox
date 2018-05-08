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
	} // el
#endif // EL_CONDITIONAL_H
