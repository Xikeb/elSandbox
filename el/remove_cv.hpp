#ifndef ELMETA_REMOVE_CV_H
#define ELMETA_REMOVE_CV_H

#include "el/remove_const.hpp"
#include "el/remove_volatile.hpp"

namespace el {
	template<typename T>
	using remove_cv_t = el::remove_const_t<el::remove_volatile_t<T>>;
} // el

#endif // EL_REMOVE_CV_H
