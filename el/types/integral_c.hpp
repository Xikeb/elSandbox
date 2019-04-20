#pragma once

#include <utility>
#include <type_traits>

#include "el/types/void.hpp"
#include "el/enable_if.hpp"

#define ENABLE_IF_HAS_UNOP(sign, name)														\
	constexpr static auto op_##name = [](auto &&a) { return sign(a); };						\
	template<typename T, T t>																\
	constexpr auto operator sign(el::integral_c<T, t>) noexcept;							\
	template<typename T, T t, typename = el::void_t<										\
		decltype(op_##name(std::declval<T&>()))												\
	>> constexpr auto operator sign(el::integral_c<T, t>) noexcept							\
	{																						\
		auto v = t;																			\
		return el::integral_c<decltype(sign(v)), sign(v)>{};								\
	}

namespace el {
	template<typename T, T t = T()>
	struct integral_c {
		using type = integral_c;
		using value_type = T;
		constexpr static value_type value = t;

		constexpr integral_c() { }
		constexpr integral_c(T&&) { }

		constexpr operator value_type() const noexcept {
			return value;
		}

		constexpr value_type operator()() const noexcept {
			return value;
		}

		// template<typename = el::void_t<decltype(!value)>>
		// constexpr auto operator!() const noexcept
		// {
		// 	return el::integral_c<decltype(!value), !value>{};
		// }

		// ENABLE_IF_HAS_BINOP(+, plus);
		// ENABLE_IF_HAS_BINOP(-, minus);
		// ENABLE_IF_HAS_BINOP(*, times);
		// ENABLE_IF_HAS_BINOP(/, div);
		// ENABLE_IF_HAS_BINOP(%, mod);

		// ENABLE_IF_HAS_BINOP(&, band);
		// ENABLE_IF_HAS_BINOP(|, bor);
		// ENABLE_IF_HAS_BINOP(^, bxor);
		// ENABLE_IF_HAS_BINOP(&&, land);
		// ENABLE_IF_HAS_BINOP(||, lor);

		// ENABLE_IF_HAS_BINOP(<, gt);
		// ENABLE_IF_HAS_BINOP(<=, geq);
		// ENABLE_IF_HAS_BINOP(==, eq);
		// ENABLE_IF_HAS_BINOP(>, lt);
		// ENABLE_IF_HAS_BINOP(>=, leq);

		// template<typename U, U u, typename = el::void_t<decltype(value > u)>>
		// constexpr auto operator>(el::integral_c<U, u>) const noexcept
		// {
		// 	return el::integral_c<decltype(value > u), value > u>{};
		// }

		//Assign
		// ENABLE_IF_HAS_BINOP(,, comma);
		// ENABLE_IF_HAS_BINOP(., dot);
	};

	ENABLE_IF_HAS_UNOP(!, lnot);
	ENABLE_IF_HAS_UNOP(~, bnot);
	ENABLE_IF_HAS_UNOP(+, uplus);
	ENABLE_IF_HAS_UNOP(-, uminus);
	// ENABLE_IF_HAS_UNOP(*, deref);
	// ENABLE_IF_HAS_UNOP(&, addrof);
	ENABLE_IF_HAS_UNOP(++, incr);
	ENABLE_IF_HAS_UNOP(--, decr);

	using true_c = el::integral_c<bool, true>;
	using false_c = el::integral_c<bool, false>;

	template<bool x>
	using bool_c = el::integral_c<bool, x>;

	template<char x>
	using char_c = el::integral_c<char, x>;

	template<short x>
	using short_c = el::integral_c<short, x>;

	template<int x>
	using int_c = el::integral_c<int, x>;

	template<long x>
	using long_c = el::integral_c<long, x>;

	template<unsigned long x>
	using size_c = el::integral_c<unsigned long, x>;
} // el
