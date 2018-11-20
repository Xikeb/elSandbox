/*
 * Manager_test.cpp
 *
 *  Created on: 23 oct. 2017
 *      Author: eliord
 */
#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include <memory>
#include <chrono>
#include <iomanip>

#include "Manager.hpp"
#include "el/static_if.hpp"
#include "el/remove_ref.hpp"
#include "el/detail/pretty_print.hpp"
#include "el/types/type_c.hpp"
#include "el/types/is_valid.hpp"
#define ECS_TAG(t)			namespace tag { constexpr static el::Type_c<t> t = {}; }
#define ECS_COMPONENT(t)		namespace comp { constexpr static el::Type_c<t> t = {}; }

using namespace std;
using namespace el::detail;
namespace test {
	struct Vector2i {
		Vector2i(int x = 0, int y = 0): x(x), y(y) {}
		int x;
		int y;
	};

	struct Vector3i {
		Vector3i(int x = 0, int y = 0, int z = 0): x(x), y(y), z(z) {}
		int x;
		int y;
		int z;
	};

	struct Vector2f {
		Vector2f(float x = 0, float y = 0): x(x), y(y) {}
		float x;
		float y;
	};

	struct Vector3f {
		Vector3f(float x = 0, float y = 0, float z = 0): x(x), y(y), z(z) {}
		float x;
		float y;
		float z;
	};

	struct Transform {
		Vector2f pos;
		Vector2f rot;
		Vector2f sca;
	};

	union Color {
		char rgba[4];
		int col;
	};

	struct Drawable {
		Color col;
		float depth;
	};

	struct Inverted;
	struct KillOnSight;
} //test

namespace test {
	using Components = ecs::ComponentList<
		Vector2f, Vector3f, Vector2i, Vector3i,
		std::string, Transform, Color, Drawable
	>;

	using Tags = ecs::TagList<Inverted, KillOnSight>;

	using Settings = ecs::Settings<Components, Tags>;

	using FullManager = ecs::Manager<test::Settings>;
	using EmptyManager = ecs::Manager<
		ecs::Settings<
			ecs::ComponentList<>,
			ecs::TagList<>
		>
	>;

	using HasString = ecs::Signature<Settings::Basic, std::string>;
} // test
using namespace test;
ECS_TAG(KillOnSight);
ECS_TAG(Inverted);
ECS_COMPONENT(Vector2f);
ECS_COMPONENT(Vector3f);
ECS_COMPONENT(Vector2i);
ECS_COMPONENT(Vector3i);
ECS_COMPONENT(string);
ECS_COMPONENT(Transform);
ECS_COMPONENT(Color);
ECS_COMPONENT(Drawable);

namespace text {
	static std::string blue = "\033[0;36m";
	static std::string red = "\033[0;31m";
	static std::string bold = "\033[1m";
	static std::string reset = "\033[00m\033[m";
} // text

template<typename T, typename ...Args>
void benchmark(T&& callable, std::size_t reps = 1000, string label = "Tested function", Args&&...args) {
	auto then = std::chrono::high_resolution_clock::now();
	for (auto i = reps; i > 0; --i)
		callable(std::forward<Args>(args)...);
	auto now = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> diff = now - then;
	cout << text::blue << "[ Benchmark] " << label << ": "
	     << std::fixed << std::setprecision(2) << diff.count()
	     << "ms (" << reps << " reps)" << text::reset << std::endl;
}

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

	std::vector<int> ivect(count);
	benchmark(
		[&ivect, i = std::make_shared<int>(0)]{
			ivect.push_back(*i);
			++*i;
		},
		count, "ivect.push_back()"
	);
	ivect.clear();
	benchmark(
		[&ivect, i = std::make_shared<int>(0)]{
			ivect.emplace_back(*i);
			++*i;
		},
		count, "ivect.emplace_back()"
	);
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

	cout << "FullManager: \t" << fstates[0] << endl << "\t\t" << fstates[1] << endl;
	cout << "EmptyManager: \t" << estates[0] << endl << "\t\t" << estates[1] << endl;
}

int main(int argc, char *argv[])
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}