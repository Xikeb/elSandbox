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

#include "Manager.hpp"
#include "el/detail/pretty_print.hpp"
#include "el/types/type_c.hpp"

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

template<typename T, typename ...Args>
void benchmark(T&& callable, std::size_t reps = 1000, string label = "Tested function", Args&&...args) {
	auto then = std::chrono::high_resolution_clock::now();
	for (auto i = reps; i > 0; --i)
		callable(std::forward<Args>(args)...);
	auto now = std::chrono::high_resolution_clock::now();
	cout << label << ": " << (then - now) << "ms (" << count << "reps)\n";
}

TEST(ManagerTest, Contruction) {
	FullManager fmgr;
	EmptyManager emgr;
}

TEST(ManagerTest, Enlarge) {
	FullManager fmgr;
	EmptyManager emgr;

	fmgr.enlarge(128);
	ASSERT_LE(128, fmgr.capacity());
	emgr.enlarge(128);
	ASSERT_LE(128, emgr.capacity());
}

TEST(ManagerTest, IsComponent) {

	ASSERT_TRUE((FullManager::isComponent<Vector2f>));
	ASSERT_TRUE((FullManager::isComponent<Transform>));
	ASSERT_TRUE((FullManager::isComponent<std::string>));

	ASSERT_FALSE((FullManager::isComponent<Inverted>));
	ASSERT_FALSE((FullManager::isComponent<std::pair<int, int>>));
	ASSERT_FALSE((FullManager::isComponent<std::unique_ptr<int>>));

	ASSERT_FALSE((EmptyManager::isComponent<Vector2f>));
	ASSERT_FALSE((EmptyManager::isComponent<Transform>));
	ASSERT_FALSE((EmptyManager::isComponent<std::string>));

	ASSERT_FALSE((EmptyManager::isComponent<Inverted>));
	ASSERT_FALSE((EmptyManager::isComponent<std::pair<int, int>>));
	ASSERT_FALSE((EmptyManager::isComponent<std::unique_ptr<int>>));
}

TEST(ManagerTest, IsTag) {
	ASSERT_TRUE((FullManager::isTag<Inverted>));

	ASSERT_FALSE((FullManager::isTag<Vector2f>));
	ASSERT_FALSE((FullManager::isTag<Transform>));
	ASSERT_FALSE((FullManager::isTag<std::string>));

	ASSERT_FALSE((FullManager::isTag<std::pair<int, int>>));
	ASSERT_FALSE((FullManager::isTag<std::unique_ptr<int>>));

	ASSERT_FALSE((EmptyManager::isTag<Inverted>));

	ASSERT_FALSE((EmptyManager::isTag<Vector2f>));
	ASSERT_FALSE((EmptyManager::isTag<Transform>));
	ASSERT_FALSE((EmptyManager::isTag<std::string>));

	ASSERT_FALSE((EmptyManager::isTag<Inverted>));
	ASSERT_FALSE((EmptyManager::isTag<std::pair<int, int>>));
	ASSERT_FALSE((EmptyManager::isTag<std::unique_ptr<int>>));
}

TEST(ManagerTest, componentTagCount) {
	ASSERT_EQ(8, FullManager::componentCount);
	ASSERT_EQ(1, FullManager::tagCount);

	ASSERT_EQ(0, EmptyManager::componentCount);
	ASSERT_EQ(0, EmptyManager::tagCount);
}

TEST(ManagerTest, createEntity) {
	FullManager fmgr;
	EmptyManager emgr;
	std::size_t count = 1000;

	fmgr.createEntity();
	fmgr.createEntity();
	fmgr.createEntity();
	ASSERT_EQ(3, fmgr.entityCount());

	for (auto i = 10000; i > 0; --i)
		fmgr.createEntity();

	emgr.createEntity();
	emgr.createEntity();
	emgr.createEntity();
	ASSERT_EQ(3, emgr.entityCount());
}


TEST(ManagerTest, killEntity) {
	FullManager fmgr;
	EmptyManager emgr;

	fmgr.createEntity();
	auto fh1 = fmgr.createEntity();
	auto fh2 = fmgr.createEntity();

	ASSERT_TRUE(fh1.isValid());
	ASSERT_TRUE(fh2.isValid());
	ASSERT_EQ(3, fmgr.entityCount());

	fh1.kill();

	ASSERT_EQ(3, fmgr.entityCount());
	ASSERT_FALSE(fh1.isValid());
	ASSERT_TRUE(fh2.isValid());

	emgr.createEntity();
	auto eh1 = emgr.createEntity();
	auto eh2 = emgr.createEntity();

	ASSERT_TRUE(eh1.isValid());
	ASSERT_TRUE(eh2.isValid());
	ASSERT_EQ(3, emgr.entityCount());

	eh1.kill();

	ASSERT_EQ(3, emgr.entityCount());
	ASSERT_FALSE(eh1.isValid());
	ASSERT_TRUE(eh2.isValid());
}

TEST(ManagerTest, refresh) {
	FullManager fmgr;
	std::size_t count = 1000;

	std::vector<int> ivect(count);
	benchmark(
		[&ivect, i = std::make_shared<int>(0)]{
			ivect.push_back(*i);
			++*i;
		},
		1000, "ivect.push_back()"
	);
	ivect.clear();
	benchmark(
		[&ivect, i = std::make_shared<int>(0)]{
			ivect.emplace(*i);
			++*i;
		},
		1000, "ivect.emplace()"
	);
}

int main(int argc, char *argv[])
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}