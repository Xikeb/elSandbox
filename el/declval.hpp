#ifndef EL_DECLVAL_H
#define EL_DECLVAL_H

namespace el {
	template<typename T>
	constexpr auto declval(T t) noexcept -> decltype(el::move(t)) {
		return el::move(t);
	}
} // el

#endif // EL_DECLVAL_H
