#ifndef ELMETA_REMOVE_CV_H
	#define ELMETA_REMOVE_CV_H
	#include "el/remove_const.hpp"
	#include "el/remove_volatile.hpp"
	#include "el/remove_ref.hpp"

	namespace el {
		template<typename T>
		struct remove_cv {
			using type = T;
		};

		template<typename T>
		struct remove_cv<const T> {
			using type = T;
		};

		template<typename T>
		struct remove_cv<const T&> {
			using type = T&;
		};

		template<typename T>
		struct remove_cv<const T&&> {
			using type = T&&;
		};

		template<typename T>
		struct remove_cv<volatile T> {
			using type = T;
		};

		template<typename T>
		struct remove_cv<volatile T&> {
			using type = T&;
		};

		template<typename T>
		struct remove_cv<volatile T&&> {
			using type = T&&;
		};

		template<typename T>
		struct remove_cv<const volatile T> {
			using type = T;
		};

		template<typename T>
		struct remove_cv<const volatile T&> {
			using type = T&;
		};

		template<typename T>
		struct remove_cv<const volatile T&&> {
			using type = T&&;
		};

		template<typename T>
		using remove_cv_t = typename el::remove_cv<T>::type;
		template<typename T>
		using remove_cvref_t = typename el::remove_ref_t<el::remove_cv_t<T>>;
	} // el
#endif // EL_REMOVE_CV_H
