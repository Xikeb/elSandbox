#ifndef EL_INTEGRAL_C_H
	#define EL_INTEGRAL_C_H

	namespace el {
		template<typename T, T t>
		struct integral_c {
			using type = integral_c;
			using value_type = T;
			constexpr static value_type value = t;

			constexpr integral_c() { }

			constexpr operator value_type() const noexcept {
				return value;
			}

			constexpr value_type operator()() const noexcept {
				return value;
			}
		};

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
#endif // EL_INTEGRAL_C_H
