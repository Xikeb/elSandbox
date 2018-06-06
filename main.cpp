#include <iostream>
#include <tuple>
#include "el/type_list/type_list.hpp"
#include "el/type_list/includes.hpp"
#include "el/detail/and.hpp"
#include "el/types/is_valid.hpp"
#include "el/types/type_c.hpp"
#include "el/remove_cv.hpp"
using namespace std;

using Likes = el::type_list<
	int, double, char, float, bool,
	short
>;

using Loves = el::type_list<int>;

#include <iostream>
template<typename T>
void pretty_print(T t __attribute__((unused))) {
	std::cout << __PRETTY_FUNCTION__ << std::endl;
}

struct Has {
	bool say(int i) { cout << "Has says " << i << endl; return true; }
};

struct Oth {
	bool say(int i __attribute__((unused))) { cout << "Oth is out" << endl; return false; }
};

struct Doesnt {

};

template<typename T>
using IsInt = typename Loves::template Contains<T>;

using NewList = typename Likes::template Filter<IsInt>;

int utests_elMeta()
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
	auto canSay = el::is_valid([](auto &&me) { return me.say(6); });
	pretty_print(canSay(Has{ }));

	cout << endl << endl;

	const int c = 0;
	volatile int v = 0;
	const int &cr = c;
	volatile int &vr = v;
	pretty_print(el::type_c<decltype(c)>); pretty_print(el::type_c<decltype(v)>);
	pretty_print(el::type_c<decltype(cr)>); pretty_print(el::type_c<decltype(vr)>);
	cout << endl;
	pretty_print(el::type_c<el::remove_const_t<decltype(c)>>); pretty_print(el::type_c<remove_volatile_t<decltype(v)>>);
	pretty_print(el::type_c<el::remove_cvref_t<decltype(cr)>>); pretty_print(el::type_c<decltype(vr)>);

	cout << "New List: "; pretty_print(NewList{});
	return 0;
}

int main() {
	return utests_elMeta();
}