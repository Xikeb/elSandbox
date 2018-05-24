#include <iostream>
#include <tuple>
#include "el/type_list/type_list.hpp"
#include "el/type_list/includes.hpp"
#include "el/detail/and.hpp"
using namespace std;

using Likes = el::type_list<
	int, double, char, float, bool,
	short
>;

template<typename T>
void pretty_print(T t __attribute__((unused))) {
	std::cout << __PRETTY_FUNCTION__ << std::endl;
}

namespace impl {
	template<typename TF, typename ...Args, typename = decltype(
		std::declval<TF>()(std::declval<Args>()...)
	)>
	constexpr auto is_valid(int) noexcept {
		return el::true_c{};
	}

	template<typename TF, typename ...Args>
	constexpr auto is_valid(...) noexcept {
		return el::false_c{};
	}

	template<typename TF>
	struct is_valid_functor {
		template<typename ...Args>
		constexpr auto operator()(Args&&...) const noexcept {
			return impl::is_valid<TF, Args...>();
		}
	};
} // impl

template<typename TF>
constexpr auto is_valid(TF&&) noexcept
{
	return impl::is_valid_functor<TF>();
}

struct Has {
	bool say(int i) { cout << "Has says " << i << endl; return true; }
};

struct Oth {
	bool say(int i __attribute__((unused))) { cout << "Oth is out" << endl; return false; }
};

struct Doesnt {

};

int main()
{
	pretty_print(el::type_list<void>{ });
	pretty_print(el::Rename<std::tuple, Likes>{ });
	cout << endl;

	cout << "<And> test:\t\t";
	pretty_print(el::detail::_and<true, true, false>{ });

	cout << "<Includes> test:\t";
	pretty_print(el::Includes<Likes, int>{ });

	cout << "<Contains(true)> test:\t";
	pretty_print(Likes::Contains<int>{ });

	cout << "<Contains(false)> test:\t";
	pretty_print(Likes::Contains<std::size_t>{ });

	cout << "<IndexOf(short)> test:\t";
	pretty_print(Likes::IndexOf<std::tuple<double, double>>{ });
	cout << endl;

	cout << "<IsEnd> test:\t\t";
	pretty_print(el::IsEnd<el::type_list<>>{ });

	cout << "<is_same> test:\t\t";
	pretty_print(el::is_same<Likes::Current, int>{ });

	cout << "<conditional> test:\t";
	pretty_print(
		el::conditional<
			el::is_same<Likes::Current, int>::value,
			el::true_c,
			el::false_c
		>::type{ }
	);
	cout << endl;

	cout << "<is_valid> test:\t";
	auto canSay = is_valid([](auto &&me) { return me.say(6); });
	pretty_print(canSay(Doesnt{ }));
	return 0;
}