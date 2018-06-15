#ifndef ELMETA_TYPE_C_HPP
	#define ELMETA_TYPE_C_HPP
	#include "el/remove_ref.hpp"
	#define TYPE_OF(t)		type_of<decltype(t)>

	namespace el {
		template<typename T>
		struct Type_c {
			using type = T;
			using This = el::Type_c<type>;

			constexpr Type_c() = default;

			constexpr bool operator==(el::Type_c<T>)
			{
				return true;
			}

			constexpr bool operator==(el::Type_c<T>&)
			{
				return true;
			}

			constexpr bool operator==(el::Type_c<T>&&)
			{
				return true;
			}

			template<typename U>
			constexpr bool operator==(el::Type_c<U>)
			{
				return false;
			}

			template<typename U>
			constexpr bool operator==(el::Type_c<U>&)
			{
				return false;
			}

			template<typename U>
			constexpr bool operator==(el::Type_c<U>&&)
			{
				return false;
			}

			auto operator+() const noexcept
			{
				return static_cast<This const &&>(*this);
			}
		};

		/*
		 * References are removed because in it's simplest
		 * case, decltype is capable of using the type of
		 * the variable, and not that of the function argument
		 * which it would be passed. cv-qualifiers are retained :
		 *	int i = 0;
		 *	static_assert(el::is_same<decltype(i), int>, "Variable type well understood");
		 *	static_assert(el::is_same<decltype(el::make_type(i)), int&>, "Function argument type well understood");
		*/
		template<typename T>
		constexpr auto make_type(T&&) noexcept
		{
			return el::Type_c<el::remove_ref_t<T>>();
		}

		template<typename T>
		constexpr static el::Type_c<T> type_c = {};

		template<typename T>
		using type_of = typename T::type;
	} // el
#endif // ELMETA_TYPE_C_HPP