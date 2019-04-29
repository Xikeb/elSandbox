#include <gtest/gtest.h>
#include <string>
#include <iostream>

#include "el/detail/pretty_print.hpp"
#include "SystemSpecs.hpp"
#include "Machinery.hpp"
#include "unit-tests-setup.hpp"

using namespace std;
using namespace test;

TEST(MachineryTest, Instantiation) {
	constexpr static auto callback = [](auto&&) {
		debug_log() << "(Lambda) Beginning of default empty system\n";
		debug_log() << "(Lambda) End of default empty system\n";
	};
	test::FullManager fmgr;
	test::EmptyManager emgr;
	auto spec = ecs::SystemSpecs{callback};
	auto fmch = ecs::Machinery{fmgr, spec};
	auto emch = ecs::Machinery{emgr, spec};

	fmch.construct(spec);
	emch.construct(spec);
	EXPECT_TRUE((fmch.isOwnSpec<decltype(spec)>));
	EXPECT_EQ((fmch.systemId<decltype(spec)>), 0);
	EXPECT_TRUE((el::is_same_v<
		el::remove_ref_t<decltype(fmch)::SpecById<0>>,
		el::remove_ref_t<decltype(spec)>
	>));
	EXPECT_TRUE((el::is_same_v<
		el::remove_ref_t<decltype(fmch.getSystem<0>())>,
		el::remove_ref_t<decltype(spec())>
	>));
	EXPECT_TRUE((el::is_same_v<
		el::remove_ref_t<decltype(fmch.getSystem<decltype(spec)>())>,
		el::remove_ref_t<decltype(spec())>
	>));
	EXPECT_TRUE((el::is_same_v<
		el::remove_ref_t<decltype(fmch.getSystem(spec))>,
		el::remove_ref_t<decltype(spec())>
	>));
}