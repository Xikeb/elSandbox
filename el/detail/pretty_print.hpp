#ifndef ELMETA_PRETTY_PRINT_HPP
	#define ELMETA_PRETTY_PRINT_HPP
	#include <iostream>

	namespace el {
		namespace detail {
			template<typename T>
			void pretty_print(T&&) {
				std::cout << __PRETTY_FUNCTION__;
			}
		} // detail
	} // el
#endif // ELMETA_PRETTY_PRINT_HPP