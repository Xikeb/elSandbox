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

std::ostream &operator<<(std::ostream &os, Vector2f const &v) {
	return os << "(" << v.x << ", " << v.y << ")";
}

namespace example {
	struct Position: public Vector2f {};
	struct Speed: public Vector2f {};

	//A tag is only a boolean stuck to the entity
	//You can refer to that boolean using the type you registered it as.
	//That type is only a name/reference to the boolean in the entity,
	//and is never instantiated
	struct IsAlien;
} // example

using namespace example;
//This is a convenience macro, that creates a little value in the comp:: namespace for the type
//you want, allowing you to use that type as you would any other value,
//which removes the need for the ugly syntax when using a template member
//method where you need to supply arguments between carets
//otherwise it would look like this:
// entity.template getComponent<example::Position>();
//Now, it looks like this:
// entity.getComponent(comp::Position);
ECS_NSCOMPONENT(example, Position);
ECS_NSCOMPONENT(example, Speed);
//Same for the tag, but the namespace is tag::
ECS_NSTAG(example, IsAlien);


//List of types to use as components in the ECS
using Components = ecs::ComponentList<example::Position, example::Speed>;

//List of types to use as tags in the ECS
using Tags = ecs::TagList<IsAlien>;

//You create settings with those 2 lists (those settings will/light contain more later, hence the extra step)
//The reason this alias exists separately is that signatures will also depend on those settings
using Settings = ecs::Settings<Components, Tags>;

//And finally create an ECS Manager with those settings ()
using Manager = ecs::Manager<Settings>;

//A signature is a automated check on the entity's metadata: rather than writing out the 
//calls to 'hasComponent' and 'hasTag'  methods, you can create a signature, which is essentially
//a type that automates those checks


//Error from last night: forgot that signatures require in their template the manager settings
using Positionable = ecs::Signature<Settings, example::Position>;

//Here's a basic signature: give it all the types it must check for (both components and tags),
//and you have now created a class that will check those for you.
//It can sort between components and tags thanks to the Settings you gave it.
//Keep it in a corner of you mind, it will be used later
using Movable = ecs::Signature<Settings, example::Position, example::Speed>;
using Targetable = ecs::Signature<Settings, example::Position, example::IsAlien>;

int main() {
	//Phew, we're finally instantiating our manager
	Manager mgr;

	//First, it'd be nice to have our bools displayed as 'true'/'false' rather than '0'/'1'
	std::cout << std::boolalpha;

	//Let's populate our world...
	for (auto i = 21; i; --i) {
		//...by creating an entity
		auto ent = mgr.createEntity();

		//That entity has a 1 in 5 chance of being on the field
		if (std::rand() % 5) {
			ent.addComponent(comp::Position, Position{10.f, 10.f});
		}
	}

	//Look at my trucker... truck
	auto junkShipEntity = mgr.createEntity();

	//Wanna see it sailing around
	//The 'import stream' operator uses the entity's methods addComponent/addTag
	junkShipEntity << Position{25.f, 50.f};

	//While it's going to the field, revel in those specs
	//Remember when we made values out of our types for a better syntax? Here's an example
	std::cout << "Is that a fucken alien!?:  " << (junkShipEntity.hasTag(tag::IsAlien) ? "Ugh..." : "Hell naw!") << std::endl;
	//the 'binary and' operator uses the methods hasComponent/hasTag
	std::cout << "Is it have somewhere on the field: " << (junkShipEntity & comp::Position) << std::endl;
	std::cout << "Does it have an engine component: " << (junkShipEntity & comp::Speed) << std::endl;
	std::cout << std::endl;

	//You can compile after this return and test it first
	//return 0;

	//Whoops, notice anything?
	//Yup, we've left our ship stranded in the middle of space with no engine.

	junkShipEntity
		>> comp::Position					//The aliens took it away it (not in space anymore). The 'export stream' operator uses removeComponent/removeTag
		<< tag::IsAlien 					//Look, they're molesting it with their dirty tentacles! It has already fallen...
		<< Speed{2.f, 2.f}					//Argh... the eggs jsut hatched! they're flailing around with their baby killer tentacles
	;

	std::cout << "Does it have the Position component: " << (junkShipEntity & comp::Position) << std::endl;
	std::cout << "Does it have the Speed component: " << (junkShipEntity & comp::Speed) << std::endl;
	std::cout << "Is that a fucking alien!?:  " << (junkShipEntity[tag::IsAlien]) << std::endl;
	std::cout << std::endl;

	//You can compile after this return and test it first
	//return 0;


	//Oh shit, look, over there!!
	junkShipEntity << Position{5.f, 0.f};

	//Oh, shit, it's moving !? wasnt it stranded !?
	auto &pos = junkShipEntity.getComponent(comp::Position);
	//The 'array access' operator uses the getComponent/hasTag methods (getTag and hasTag are equivalent)
	auto &spd = junkShipEntity[comp::Speed];
	//Wait, if I put this x and x together, and then this y and that y together...
	//Isnt it coming straight towards us !?
	pos.x += spd.x;
	pos.y += spd.y;

	std::cout
		<< "Position" << pos
		<< "Speed" << spd
		<< std::endl
	;

	//Let's make things a bit more lively
	for (auto i = 8; i; --i) {
		if (std::rand() % 5)
			mgr.createEntity();
		else
			mgr.createEntity() << Position{0.f, 0.f};
	}

	//So that the previous fumble doesn't happen, let's setup something that quickly calculates positions of moving objects
	constexpr static auto fMove = [](auto &ent) { ent[comp::Position] += ent[comp::Speed]; };

	//This watches everythin in sight and shows it's position and speed. We'll never be surprised again
	constexpr static auto fPos = [](const auto &ent) {
		std::cout << "\tSighted at " << ent[comp::Position];
		
		if (ent & comp::Speed) {
			std::cout << " moving at " << ent[comp::Speed];
		}

		std::cout << std::endl;
	};

	//This creates a blueprint for a system, a SystemSpec (or system specification), something that can be run over
	
	// auto moveSytemBlueprintPart1 = ecs::SystemSpecs{fMove};
	
	//But it has a problem: it assumes the entity has a Position and Speed
	//If it doesn't, you'd be lucky to have it crash on the spot
	//Lets make sure it has all the component it needs to take action

	// using Movable = ecs::Signature<Settings, example::Position, example::Speed>;
	
	//Same as above, basically, we are creating a class that will check that every
	//element we need is present. 
	//Now, we can create a second version of our blueprint (those SystemSpecs are immutable,
	//will create a ne object. do use auto though, writing out the template argument is pretty
	//much always a pain in the rear here. Anything for that sweet performance, though.)

	// auto moveSytemBlueprintPart2 = moveSytemBlueprintPart1.matches(el::type_c<Movable>);

	auto moveSpec = ecs::SystemSpecs{fMove}
		.template matching<Movable>();
	auto moveSys = moveSpec();

	auto posSpec = ecs::SystemSpecs{fPos}
		.template matching<Positionable>();
	auto posSys = posSpec();

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