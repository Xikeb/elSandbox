#include <iostream>
#include <type_traits>
#include <functional>

#include "el/types/type_c.hpp"

using namespace std;

template<typename T, template <typename> class ...Fs>
struct Applicate;

template<typename T, template <typename> class F, template <typename> class ...Fs>
struct Applicate<T, F, Fs...> {
	using Type = typename Applicate<F<T>, Fs...>::Type;
};

template<typename T>
struct Applicate<T> {
	using Type = T;
};

template<template <typename> class ...Fs>
constexpr static bool hasany = true;

template<typename T, template <typename> class F, template <typename> class ...Fs>
using applicate = typename Applicate<T, F, Fs...>::Type;
// using applicate = std::enable_if<hasany<Fs...>, F<T>>;

template<typename T>
void pretty_print(T&&) {
	std::cout << __PRETTY_FUNCTION__ << std::endl;
}

struct Child: Applicate<void, decay_t>
{
};

int main(int argc, char const *argv[])
{	
	const auto callback = [](int) {return 0;}; hasany<function, decay_t, remove_pointer_t>;
	auto i = el::type_c<applicate<decltype(callback), decay_t, remove_pointer_t, function>>;
	pretty_print(i);
	pretty_print(Child::applicate);
	return 0;
}

#include "el/types/integral_c.hpp"
using test_c = el::int_c<42>;

constexpr static el::int_c<42> test_int = el::int_c<42>{};
constexpr static el::bool_c<false> test_bool = el::bool_c<false>{};