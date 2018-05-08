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
	return 0;
}