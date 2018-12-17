/*
 * Manager_test.cpp
 *
 *  Created on: 23 oct. 2017
 *      Author: eliord
 */
#include <gtest/gtest.h>
#include <iostream>
#include <string>

#include "el/static_if.hpp"
#include "el/types/type_c.hpp"
#include "el/types/is_valid.hpp"
#include "Manager.hpp"
#include "unit-tests-setup.hpp"

using namespace test;
using namespace std;
using namespace el::detail;

TEST(ManagerTest, Contruction) {
	FullManager fmgr;
	EmptyManager emgr;
}

TEST(ManagerTest, Enlarge) {
	FullManager fmgr;
	EmptyManager emgr;

	fmgr.enlarge(8192);
	EXPECT_LE(8192, fmgr.capacity());
	emgr.enlarge(8192);
	EXPECT_LE(8192, emgr.capacity());
}

TEST(ManagerTest, IsComponent) {

	EXPECT_TRUE((FullManager::isComponent<Vector2f>));
	EXPECT_TRUE((FullManager::isComponent<Transform>));
	EXPECT_TRUE((FullManager::isComponent<std::string>));

	EXPECT_FALSE((FullManager::isComponent<Inverted>));
	EXPECT_FALSE((FullManager::isComponent<std::pair<int, int>>));
	EXPECT_FALSE((FullManager::isComponent<std::unique_ptr<int>>));

	EXPECT_FALSE((EmptyManager::isComponent<Vector2f>));
	EXPECT_FALSE((EmptyManager::isComponent<Transform>));
	EXPECT_FALSE((EmptyManager::isComponent<std::string>));

	EXPECT_FALSE((EmptyManager::isComponent<Inverted>));
	EXPECT_FALSE((EmptyManager::isComponent<std::pair<int, int>>));
	EXPECT_FALSE((EmptyManager::isComponent<std::unique_ptr<int>>));
}

TEST(ManagerTest, IsTag) {
	EXPECT_TRUE((FullManager::isTag<Inverted>));

	EXPECT_FALSE((FullManager::isTag<Vector2f>));
	EXPECT_FALSE((FullManager::isTag<Transform>));
	EXPECT_FALSE((FullManager::isTag<std::string>));

	EXPECT_FALSE((FullManager::isTag<std::pair<int, int>>));
	EXPECT_FALSE((FullManager::isTag<std::unique_ptr<int>>));

	EXPECT_FALSE((EmptyManager::isTag<Inverted>));

	EXPECT_FALSE((EmptyManager::isTag<Vector2f>));
	EXPECT_FALSE((EmptyManager::isTag<Transform>));
	EXPECT_FALSE((EmptyManager::isTag<std::string>));

	EXPECT_FALSE((EmptyManager::isTag<Inverted>));
	EXPECT_FALSE((EmptyManager::isTag<std::pair<int, int>>));
	EXPECT_FALSE((EmptyManager::isTag<std::unique_ptr<int>>));
}

TEST(ManagerTest, componentTagCount) {
	EXPECT_EQ(8, FullManager::componentCount);
	EXPECT_EQ(2, FullManager::tagCount);

	EXPECT_EQ(0, EmptyManager::componentCount);
	EXPECT_EQ(0, EmptyManager::tagCount);
}

TEST(ManagerTest, createEntity) {
	FullManager fmgr;
	EmptyManager emgr;
	std::size_t count = 100000;
	constexpr static auto createEntity = [](auto&& mgr) {mgr.createEntity();};

	fmgr.createEntity();
	fmgr.createEntity();
	fmgr.createEntity();
	EXPECT_EQ(3, fmgr.entityCount());
	benchmark(createEntity, count, "FullManager.createEntity", fmgr);

	emgr.createEntity();
	emgr.createEntity();
	emgr.createEntity();
	EXPECT_EQ(3, emgr.entityCount());
	benchmark(createEntity, count, "EmptyManager.createEntity", emgr);
}


TEST(ManagerTest, killEntity) {
	FullManager fmgr;
	EmptyManager emgr;

	fmgr.createEntity();
	auto fh1 = fmgr.createEntity();
	auto fh2 = fmgr.createEntity();

	EXPECT_TRUE(fh1.isValid());
	EXPECT_TRUE(fh2.isValid());
	EXPECT_EQ(3, fmgr.entityCount());

	fh1.kill();

	EXPECT_EQ(3, fmgr.entityCount());
	EXPECT_FALSE(fh1.isValid());
	EXPECT_TRUE(fh2.isValid());

	emgr.createEntity();
	auto eh1 = emgr.createEntity();
	auto eh2 = emgr.createEntity();

	EXPECT_TRUE(eh1.isValid());
	EXPECT_TRUE(eh2.isValid());
	EXPECT_EQ(3, emgr.entityCount());

	eh1.kill();

	EXPECT_EQ(3, emgr.entityCount());
	EXPECT_FALSE(eh1.isValid());
	EXPECT_TRUE(eh2.isValid());
}

TEST(ManagerTest, refresh) {
	FullManager fmgr;
	EmptyManager emgr;
	std::vector<decltype(fmgr.createEntity())> fents;
	std::vector<decltype(emgr.createEntity())> eents;
	std::size_t count = 100000;

	count = 30;
	for (auto i = 0u; i < count; ++i) {
		fents.push_back(fmgr.createEntity());
		eents.push_back(emgr.createEntity());
		fents.back().template addComponent<std::string>((char[2]){static_cast<char>(static_cast<char>('0') + static_cast<char>(i)), 0});
		// eents.back().template addComponent<std::string>(std::to_string(i));
		if (i % 5 == 0) {
			// fents.back().kill();
			fents.back().template addTag<KillOnSight>();
			eents.back().kill();
		}
	}

	constexpr static auto writeValid = [](auto &&e, std::string &state) {
		constexpr static auto hasString = el::is_valid([](decltype(fmgr.createEntity()) &e) { e.template hasComponent<std::string>(); });
		if (!e.isValid())
			state += text::bold + text::red;
		state += el::static_if(hasString(e))
			.then([&state](auto &&e) {
				std::string r;
				EXPECT_TRUE(e.isValid());
				if (!e.isValid())
					return r;
				if (e.template hasTag<KillOnSight>())
					r += text::red + text::bold;
				r += e.template getComponent<std::string>();
				if (e.template hasTag<KillOnSight>())
					r += text::reset;
				return r;
			})
			.otherwise([&state](auto &&e) {
				return "01"[e.isValid()];
			})(e);
		if (!e.isValid())
			state += text::reset;
	};

	std::string fstates[3] = { "", "" };
	std::string estates[3] = { "", "" };
	// for (auto &&e: fents) fstates[0] += "01"[e.isValid()];
	// for (auto &&e: eents) estates[0] += "01"[e.isValid()];

	fmgr.forEntities(writeValid, fstates[0]);
	emgr.forEntities(writeValid, estates[0]);
	fmgr.forEntities([](auto &&e) { if (e.template hasTag<KillOnSight>()) e.kill(); });
	fmgr.refresh();
	fmgr.forEntities(writeValid, fstates[1]);
	emgr.forEntities(writeValid, estates[1]);
	EXPECT_EQ(count - count / 5, fmgr.entityCount());

	cout << "FullManager: \t" << fstates[0] << endl << "\t\t" << fstates[1] << endl;
	cout << "\t\t"; for (auto &&e: fents) cout << ("01"[e.isValid()]); cout << endl;
	cout << "EmptyManager: \t" << estates[0] << endl << "\t\t" << estates[1] << endl;
}