#ifndef EL_MOVE_H
# define EL_MOVE_H

#include "el/remove_ref.hpp"

namespace el {
	template <typename T>
	constexpr auto move(T&& t) const noexcept {
		return static_cast<el::remove_ref_t<T>&&>(t);
	}
}

#endif /*EL_MOVE_H*/