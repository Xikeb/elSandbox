#include <cstdlib>
#include <iostream>

#include "el/static_if.hpp"
#include "el/remove_ref.hpp"
#include "el/detail/pretty_print.hpp"
#include "el/types/type_c.hpp"

#include "Settings.hpp"
#include "Manager.hpp"
#include "EntityHandle.hpp"
#include "SystemSpecs.hpp"
#include "System.hpp"

#define ECS_TAG(T)		namespace tag {constexpr static auto T = el::type_c<T>;}
#define ECS_NSTAG(NS, T)	namespace tag {constexpr static auto T = el::type_c<NS :: T>;}
#define ECS_COMPONENT(T)	namespace comp {constexpr static auto T = el::type_c<T>;}
#define ECS_NSCOMPONENT(NS, T)	namespace comp {constexpr static auto T = el::type_c<NS :: T>;}

struct Vector2f {
	float x, y;

	Vector2f operator+(Vector2f const &oth) { return Vector2f{this->x + oth.x, this->y + oth.y}; }
	Vector2f operator-(Vector2f const &oth) { return Vector2f{this->x - oth.x, this->y - oth.y}; }
	Vector2f operator*(Vector2f const &oth) { return Vector2f{this->x * oth.x, this->y * oth.y}; }
	Vector2f operator/(Vector2f const &oth) { return Vector2f{this->x / oth.x, this->y / oth.y}; }

	Vector2f &operator+=(Vector2f const &oth) { this->x += oth.x; this->y += oth.y; return *this; }
	Vector2f &operator-=(Vector2f const &oth) { this->x -= oth.x; this->y -= oth.y; return *this; }
	Vector2f &operator*=(Vector2f const &oth) { this->x *= oth.x; this->y *= oth.y; return *this; }
	Vector2f &operator/=(Vector2f const &oth) { this->x /= oth.x; this->y /= oth.y; return *this; }

	Vector2f operator+(float oth) { return Vector2f{this->x + oth, this->y + oth}; }
	Vector2f operator-(float oth) { return Vector2f{this->x - oth, this->y - oth}; }
	Vector2f operator*(float oth) { return Vector2f{this->x * oth, this->y * oth}; }
	Vector2f operator/(float oth) { return Vector2f{this->x / oth, this->y / oth}; }

	Vector2f &operator+=(float oth) { this->x += oth; this->y += oth; return *this; }
	Vector2f &operator-=(float oth) { this->x -= oth; this->y -= oth; return *this; }
	Vector2f &operator*=(float oth) { this->x *= oth; this->y *= oth; return *this; }
	Vector2f &operator/=(float oth) { this->x /= oth; this->y /= oth; return *this; }

};

namespace example {
	struct Position: public Vector2f {};
	struct Speed: public Vector2f {};

	struct IsAlien;
} // example

using namespace example;

ECS_NSCOMPONENT(example, Position);
ECS_NSCOMPONENT(example, Speed);
ECS_NSTAG(example, IsAlien);


using Components = ecs::ComponentList<example::Position, example::Speed>;
using Tags = ecs::TagList<IsAlien>;
using Settings = ecs::Settings<Components, Tags>;
using Manager = ecs::Manager<Settings>;

//Error from last night: forgot that signatures require in their template the manager settings
using Positionable = ecs::Signature<Settings, example::Position>;
using Movable = ecs::Signature<Settings, example::Position, example::Speed>;
using Targetable = ecs::Signature<Settings, example::Position, example::IsAlien>;

int main() {
	Manager mgr;

	for (auto i = 21; i; --i) {
		if (std::rand() % 5)
			mgr.createEntity();
		else
			mgr.createEntity() << Position{0.f, 0.f};
	}

	auto entity = mgr.createEntity();
	std::cout << std::boolalpha;

	entity << Position{5.f, 0.f};

	std::cout << "Does it have the Position component: " << (entity & comp::Position) << std::endl;
	std::cout << "Does it have the Speed component: " << (entity & comp::Speed) << std::endl;
	std::cout << "Is that a fucking alien!?:  " << (entity & tag::IsAlien) << std::endl;
	std::cout << std::endl;


	entity >> comp::Position << Speed{2.f, 2.f} << tag::IsAlien;

	std::cout << "Does it have the Position component: " << (entity & comp::Position) << std::endl;
	std::cout << "Does it have the Speed component: " << (entity & comp::Speed) << std::endl;
	std::cout << "Is that a fucking alien!?:  " << (entity[tag::IsAlien]) << std::endl;
	std::cout << std::endl;

	entity << Position{5.f, 0.f};

	auto &pos = entity[comp::Position];
	auto &spd = entity[comp::Speed];
	pos.x += spd.x;
	pos.y += spd.y;

	std::cout
		<< "Position(" << pos.x << ", " << pos.y << ") "
		<< "Speed(" << spd.x << ", " << spd.y << ")"
		<< std::endl;

	for (auto i = 8; i; --i) {
		if (std::rand() % 5)
			mgr.createEntity();
		else
			mgr.createEntity() << Position{0.f, 0.f};
	}

	constexpr static auto fPos = [](const auto &ent) {
		const auto &pos = ent[comp::Position];
		std::cout << "\tSighted at (" << pos.x << ", " << pos.y << ")";
		
		if (ent & comp::Speed) {
			const auto &spd = ent[comp::Speed];
			std::cout << " moving at (" << spd.x << ", " << spd.y << ")";
		}

		std::cout << std::endl;
	};
	auto posSpec = ecs::SystemSpecs{fPos}
		.template matching<Positionable>();
	auto posSys = posSpec();

	constexpr static auto fMove = [](auto &ent) { ent[comp::Position] += ent[comp::Speed]; };
	auto moveSpec = ecs::SystemSpecs{fMove}
		.template matching<Movable>();
	auto moveSys = moveSpec();

	constexpr static auto fTarget = [](auto&&) { std::cout << "Ouch!\n"; };
	auto targetSpec = ecs::SystemSpecs{fTarget}
		.template matching<Targetable>();
	auto targetSys = targetSpec();


	std::cout << "Confirming current positions...\n";
	posSys(mgr);
	std::cout << std::endl;

	std::cout << "Beginning of trajectory calculation system\n";
	moveSys(mgr);
	std::cout << "End of trajectory calculation system\n";

	std::cout << "Confirming current positions...\n";
	posSys(mgr);
	std::cout << std::endl;

	std::cout << "Beginning of self defense system\n";
	targetSys(mgr);
	std::cout << "End of self defense system\n";

	return 0;
}