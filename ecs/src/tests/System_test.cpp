/*
 * Entity_test.cpp
 *
 *  Created on: 25 sept. 2017
 *      Author: eliord
 */

#include <gtest/gtest.h>
#include "unit-tests/setup.hpp"

// TEST(SystemTest, IntegerImage) {
// 	constexpr static auto printCallback = [](auto &e, auto &count) {
// 		std::cout << '\t';
// 		if (e.template hasComponent<std::string>())
// 			std::cout << e.template getComponent<std::string>();
// 		else
// 			std::cout << text::bold << "No std::string component!";
// 		std::cout << std::endl;
// 		++count;
// 	};
// 	auto print = ecs::makeSystem(printCallback)
// 		.template instantiateWith<int>()
// 		.after(el::type_c<el::type_list<>>)
// 		.matching(el::type_c<test::HasString>)()
// 	;
// }