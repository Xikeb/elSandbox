#include "Manager.hpp"
#include "Entity.hpp"
#include "el/detail/pretty_print.hpp"
#include <iostream>
#include <string>
#include <type_traits>
using namespace std;

#include "Scheduler.hpp"

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

	using Components = ecs::ComponentList<
		Vector2f, Vector3f, Vector2i, Vector3i,
		std::string, Transform, Color, Drawable
	>;

	using Tags = ecs::TagList<Inverted>;

	using Settings = ecs::Settings<Components, Tags>;

	using HasString = ecs::Signature<Settings::Basic, std::string>;
} // test
using namespace test;

#include "System.hpp"
#include "SystemSpecs.hpp"

namespace sys {
	constexpr auto printCallback = 
		[](auto &e, auto &count) {
			std::cout << '\t';
			if (e.template hasComponent<std::string>())
				std::cout << e.template getComponent<std::string>();
			else
				std::cout << "No std::string component!";
			std::cout << std::endl;
			++count;
		};
	auto print = ecs::makeSystem(printCallback)
		.template instantiateWith<int>()
		.after(el::type_c<el::type_list<>>)
		.matching(el::type_c<test::HasString>)()
	;
} // sys

int main()
{
	cout << boolalpha;

	using Manager = ecs::Manager<test::Settings>;
	Manager mgr;
	cout << "Tags:\t";
	el::detail::pretty_print(el::type_c<Manager::Settings::TagList>);
	cout << endl;
	cout << "IsTag: " << Manager::isTag<test::Inverted> << endl;
	cout << endl;


	//pretty_print(Manager::componentId<Transform>);
	for (int i = 0; i < 100; ++i) {
		auto e = mgr.createEntity();
		e.template addComponent<Vector2f>(5, 5);
		e.template addTag<Inverted>();
		e.template addComponent<std::string>("Hello");
		if (i % 2 == 0)
			e.template removeComponent<Vector2f>();
		if (i % 3 == 0)
			e.template removeTag<Inverted>();
	}
	mgr.forEntitiesMatching(HasString(), [](auto &e) {
		e.template getComponent<std::string>() += " World!";
	});
	std::size_t idx = 0;
	mgr.forEntities([&](auto &e, size_t &idx) {
		e.template removeTag<Inverted>();
		if (idx % 2 == 1)
			e.kill();
		++idx;
	}, idx);
	mgr.forEntitiesMatching(HasString(), [&](auto &e, std::string const &) {
		// cout << intro << ": " << e.template getComponent<std::string>() << endl;
		cout << e.template hasComponent<Vector2f>()
			<< " " << e.template hasTag<Inverted>()
			<< " " << e.template getComponent<std::string>()
			<< "("
				<< "inPhase: " << e.isInPhase()
				<< ", valid: " << e.isValid()
			<< ")"
		<< endl;
	}, "I say");
	cout << endl << "Print system launch: " << endl;
	sys::print(mgr);
	cout << "Printing system has worked on [" << sys::print.image() << "] entities" << endl;
	cout << "Trivially constructible:";
	return 0;
}
