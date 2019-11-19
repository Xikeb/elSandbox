/*
 * EntityHandle_test.cpp
 *
 *  Created on: 23 oct. 2017
 *      Author: eliord
 */

#include <gtest/gtest.h>

#include "el/types/type_c.hpp"
#include "el/detail/pretty_print.hpp"

#include "EntityHandle.hpp"
#include "unit-tests-setup.hpp"

using namespace std;
using namespace test;

TEST(EntityHandleTest, Construction) {
	FullManager fmgr;
	auto ent = fmgr.createEntity();

	EXPECT_TRUE(ent.isValid());
	EXPECT_EQ(&ent.getManager(), &fmgr);
	FullManager::Settings::ComponentList::for_each(
		[&ent](auto &&type, auto &&/*index*/) {
			EXPECT_FALSE(ent.template hasComponent<TYPE_OF(type)>());
		}
	);
	FullManager::Settings::TagList::for_each(
		[&ent](auto &&type, auto &&/*index*/) {
			EXPECT_FALSE(ent.template hasTag<TYPE_OF(type)>());
		}
	);
}

TEST(EntityHandleTest, Kill) {
	FullManager fmgr;
	auto ent = fmgr.createEntity();
	auto copyAlive = ent;

	ASSERT_TRUE(ent.isValid());
	ASSERT_TRUE(copyAlive.isValid());
	EXPECT_EQ(ent.getPhase(), copyAlive.getPhase());

	ent.kill();

	EXPECT_FALSE(ent.isValid());
	EXPECT_FALSE(copyAlive.isValid());
	EXPECT_EQ(ent.getPhase(), copyAlive.getPhase());

	auto second = fmgr.createEntity();
	fmgr.refresh();

	EXPECT_EQ(ent.getPhase(), second.getPhase());
	EXPECT_FALSE(ent.isValid());
	EXPECT_TRUE(second.isValid());
}

TEST(EntityHandleTest, matchesSignature) {
	FullManager fmgr;
	auto ent = fmgr.createEntity();
	std::string str = "Hello, darkness my old friend...";

	EXPECT_FALSE(ent.template matchesSignature<test::HasString>());
	EXPECT_FALSE(ent.matchesSignature(test::HasString{}));

	EXPECT_TRUE(ecs::detail::is_settings<test::Settings>);
	EXPECT_FALSE(ecs::detail::is_basic_settings<test::Settings>);

	EXPECT_TRUE(ecs::detail::is_basic_settings<test::Settings::Basic>);
	EXPECT_FALSE(ecs::detail::is_settings<test::Settings::Basic>);

	ent.template addComponent<std::string>(str);
	EXPECT_TRUE(ent.template matchesSignature<test::HasString>());
	EXPECT_TRUE(ent.matchesSignature(test::HasString{}));
	EXPECT_TRUE(ent.template hasComponent<std::string>());
	EXPECT_TRUE(ent.hasComponent(comp::string));
	EXPECT_TRUE(ent & comp::string);
	EXPECT_EQ(ent[comp::string], str);

	size_t count = 100000000;
	benchmark([]{
		EXPECT_TRUE(true);
	}, count, "assert(true)");
	benchmark([&ent]{
		EXPECT_TRUE(ent.template matchesSignature<test::HasString>());
	}, count, "ent.template matchesSignature<test::HasString>()");
	benchmark([&ent]{
		EXPECT_TRUE(ent.matchesSignature(test::HasString{}));
	}, count, "ent.matchesSignature(test::HasString{})");
}

TEST(EntityHandleTest, hasComponentTag) {
	FullManager fmgr;
	auto ent = fmgr.createEntity();
	std::string str = "Hello, darkness my old friend...";

	EXPECT_FALSE(ent.template hasComponent<std::string>());
	EXPECT_FALSE(ent.hasComponent(comp::string));
	EXPECT_FALSE(ent & comp::string);

	ent.template addComponent<std::string>(str);
	EXPECT_TRUE(ent.template hasComponent<std::string>());
	EXPECT_TRUE(ent.hasComponent(comp::string));
	EXPECT_TRUE(ent & comp::string);
	EXPECT_EQ(ent[comp::string], str);

	size_t count = 10000000;
	benchmark([&ent]{
		ent.template hasComponent<std::string>();
	}, count, "ent.template hasComponent<std::string>()");
	benchmark([&ent]{
		ent.hasComponent(comp::string);
	}, count, "ent.hasComponent(comp::string)");
	benchmark([&ent]{
		ent & comp::string;
	}, count, "ent & comp::string");
	benchmark([&ent]{
		ent.template getComponent<std::string>();
	}, count, "ent.template getComponent<std::string>()");
	benchmark([&ent]{
		ent[comp::string];
	}, count, "ent[comp::string]");
}