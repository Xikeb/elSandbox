#pragma once
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

#define ECS_TAG(T)		namespace tag {constexpr static auto T = el::type_c<T>;}
#define ECS_NSTAG(NS, T)	namespace tag {constexpr static auto T = el::type_c<NS :: T>;}
#define ECS_COMPONENT(T)	namespace comp {constexpr static auto T = el::type_c<T>;}
#define ECS_NSCOMPONENT(NS, T)	namespace comp {constexpr static auto T = el::type_c<NS :: T>;}

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
ECS_NSTAG(test, Inverted);
ECS_NSTAG(test, KillOnSight);
ECS_NSCOMPONENT(test, Vector2i);
ECS_NSCOMPONENT(test, Vector2f);
ECS_NSCOMPONENT(test, Vector3i);
ECS_NSCOMPONENT(test, Vector3f);
ECS_NSCOMPONENT(test, Transform);
ECS_NSCOMPONENT(test, Color);
ECS_NSCOMPONENT(test, Drawable);
ECS_NSCOMPONENT(std, string);

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

	using HasString = ecs::Signature<Settings, std::string>;
} // test

namespace text {
	static std::string blue = "\033[0;36m";
	static std::string red = "\033[0;31m";
	static std::string bold = "\033[1m";
	static std::string reset = "\033[00m\033[m";
} // text

inline auto &debug_log(std::string const &label = "Debug") noexcept(noexcept(std::cout << std::declval<std::string>()))
{
	auto w = std::cout.width();
	return (std::cout << text::blue << "["
			<< std::setw(10) << label
			<< "] " << std::setw(w)
	);
}

template<typename T>
inline auto &debug_log(T const &t, std::string const &label = "") noexcept(noexcept(std::cout << std::declval<std::string>()))
{
	auto w = std::cout.width();
	if (label.length()) {
		std::cout << text::blue << "["
				<< std::setw(10) << label
				<< std::setw(w) << "]: " 
		;
	}
	return std::cout << t << std::endl;
}

template<typename T, typename ...Args>
void benchmark(T&& callable, std::size_t reps = 1000, std::string label = "Tested function", Args&&...args) {
	auto then = std::chrono::high_resolution_clock::now();
	for (auto i = reps; i > 0; --i)
		callable(std::forward<Args>(args)...);
	auto now = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> diff = now - then;
	std::cout << text::blue << "[ Benchmark] " << label << ": "
		  << std::fixed << std::setprecision(2) << diff.count()
		  << "ms (" << reps << " reps)" << text::reset << std::endl;
}