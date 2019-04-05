/*
 * Entity_test.cpp
 *
 *  Created on: 25 sept. 2017
 *      Author: eliord
 */

#include <functional>
#include <gtest/gtest.h>
#include "SystemSpecs.hpp"
#include "Signature.hpp"
#include "unit-tests-setup.hpp"

void myUselessFunction(test::EmptyManager const &)
{
	debug_log() << "(Function) Beginning of default empty system\n";
	debug_log() << "(Function) End of default empty system\n";
}

TEST(SystemTest, Default) {
	test::EmptyManager emgr;
	constexpr static auto callback = [](auto&&) {
		debug_log() << "(Lambda) Beginning of default empty system\n";
		debug_log() << "(Lambda) End of default empty system\n";
	};
	ecs::SystemSpecs spec{callback};
	auto &&lambdaSys = spec();
	auto &&functionSys = spec.execution(myUselessFunction)();
	lambdaSys(emgr);
	functionSys(emgr);
}

TEST(SystemTest, ImageValue) {
	test::EmptyManager fmgr;
	std::string const name{"Xikeb"};
	const static auto callback = [&name](auto &&, auto &img) {
		static_assert((std::is_same<
			el::remove_ref_t<decltype(img)>,
			std::string
		>::value), "The image the callback obtains is of a wrong type.");
		EXPECT_EQ(img, name);
	};
	auto spec = ecs::SystemSpecs{callback}
		.instantiateWith(el::type_c<std::string>);
	auto &&sys = spec(name);
	sys(fmgr);
}

TEST(SystemTest, SignatureMatch) {
	test::EmptyManager emgr;
	test::FullManager fmgr;
	size_t count = 30;
	size_t census = 0;
	const static auto callback = [&census](auto&&) {
		debug_log("NamedEntity"); ++census;
	};
	auto spec = ecs::SystemSpecs{callback};
	auto &&sysMatching = spec.matching(el::type_c<test::HasString>)();

	size_t ecount = 0, fcount = 0;
	for (size_t i = 0; i < count; ++i) {
		emgr.createEntity(); ++ecount;

		if (i % 3 == 0) {
			fmgr.createEntity().addComponent(comp::string); ++fcount;
		} else
			fmgr.createEntity();
	}

	census = 0;
	debug_log(fcount, "fcount");
	sysMatching(fmgr);
	EXPECT_EQ(census, count/3 + 1);
	debug_log(census, "census");

	census = 0;
	debug_log(ecount, "ecount");
	debug_log(census, "census");
	sysManual(emgr);
	EXPECT_EQ(census, count);
	debug_log(census, "census");
}

TEST(SystemTest, ImageAndSignature) {
	test::EmptyManager emgr;
	test::FullManager fmgr;
	std::vector<test::EmptyManager::Handle> ehds;
	std::vector<test::FullManager::Handle> fhds;
	size_t count = 25;
	
	const auto callback = [](auto &&ent, auto &&str) {
		str += ent[comp::string];
	};
	auto spec = ecs::SystemSpecs{callback}.instantiateWith(el::type_c<std::string>);
	auto &&fsys = spec(""); (void) fsys;
	auto &&esys = spec(""); (void) esys;
	while(count--) {
		ehds.push_back(emgr.createEntity());
		fhds.push_back(fmgr.createEntity().addComponent(comp::string, std::to_string(count)));
	}
}

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
