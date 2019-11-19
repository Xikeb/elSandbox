#pragma once
#include <tuple>
#include <functional>
#include <type_traits>

#include "el/is_same.hpp"
#include "el/types/type_c.hpp"
#include "el/type_list/type_list.hpp"

#include "Settings.hpp"
#include "SystemSpecs.hpp"
#include "System.hpp"
#include "Manager.hpp"

#define ECS_UNKNOWN_SPEC		"Those specifications do not represent a System of this Machine."
#define ECS_SPEC_OUT_OF_BOUNDS	"System index out of bounds."

namespace ecs {
	namespace impl {
		template<typename Spec>
		struct Chipset {
			using type = Spec;
			using System = decltype(std::declval<type>().system());
			using Dependencies = typename Spec::Dependencies;
			template<typename T>
			using SystemOf = decltype(std::declval<T>().system());
			using SystemStorage = std::aligned_storage_t<
				sizeof(SystemOf<Spec>),
				alignof(SystemOf<Spec>)
			>;

			constexpr static size_t dependencyCount = Dependencies::size;

			template<typename T, typename = el::enable_if_t<el::is_same_v<T, Spec>>>
			constexpr auto const &get() const noexcept { return this->system; }

			SystemStorage system;
			size_t remainingDependencies;
		};
	} // impl

	namespace impl {
		template<typename T, typename ...Specs>
		struct dependents {};
	} // impl
} // ecs

namespace ecs {
	template<typename TSettings, typename ...Specs>
	struct Machinery {
		using Self = ecs::Machinery<TSettings, Specs...>;
		using Settings = TSettings;
		using Manager = ecs::Manager<Settings>;
		using Specifications = el::type_list<Specs...>;

		template<typename T>
		using SystemOf = decltype(std::declval<T>().system());
		template<size_t Id>
		using SpecById = typename Specifications::template At<Id>;
		template<std::size_t Id>
		using SystemById = SystemOf<SpecById<Id>>;

		template<typename Spec>
		using SystemStorage = std::aligned_storage_t<
			sizeof(SystemOf<Spec>),
			alignof(SystemOf<Spec>)
		>;

		template<typename Spec>
		constexpr static size_t specId = Specifications::template IndexOf<Spec>::value;
		template<typename Spec>
		constexpr static bool isOwnSpec = Specifications::template Contains<Spec>::value;

		struct is_not_void {
			template<typename T>
			constexpr static auto value = !el::is_same<
				typename SystemOf<T>::Signature,
				void
			>{};

			template<typename T>
			constexpr auto operator()() const noexcept { return value<T>; }

			template<typename T>
			constexpr auto operator()(el::Type_c<T>) const noexcept { return value<T>; }
		};

		using AutomaticSystems = typename Specifications::template Filter<is_not_void>;

		static_assert(
			ecs::detail::is_settings<Settings>,
			"Machinery requires ecs::Settings to be properly constructed."
		);
		/* Don't make that check as the clash of 2 specs with the same prototype and deps
			can only be resolved by making a child class inherit from the spec in order to give it a new name
		constexpr static auto isOnlySpecs = Specifications::for_each([](auto &&specs, auto&&){
			static_assert(
				ecs::detail::is_system_specs<TYPE_OF(specs)>,
				"Each Specification must be an instance of the ecs::SystemSpecs template."
			);
		});
		*/

		constexpr Machinery(Manager &mgr) noexcept:
		manager(mgr), constructed(0)
		{
		}

		constexpr Machinery(Manager &mgr, Specs const &...) noexcept:
		manager(mgr), constructed(0)
		{
		}

		template<typename Spec>
		auto &getSystem(Spec) noexcept {
			static_assert(isOwnSpec<Spec>, ECS_UNKNOWN_SPEC);
			return this->system<Spec>();
		}

		template<typename Spec>
		auto const &getSystem(Spec) const noexcept {
			static_assert(isOwnSpec<Spec>, ECS_UNKNOWN_SPEC);
			return this->system<Spec>();
		}

		template<typename Spec>
		auto &getSystem() noexcept {
			static_assert(isOwnSpec<Spec>, ECS_UNKNOWN_SPEC);
			return this->system<Spec>();
		}

		template<typename Spec>
		auto const &getSystem() const noexcept {
			static_assert(isOwnSpec<Spec>, ECS_UNKNOWN_SPEC);
			return this->system<Spec>();
		}

		template<size_t Id>
		auto &getSystem() noexcept {
			static_assert(Id < Specifications::size, ECS_SPEC_OUT_OF_BOUNDS);
			return this->system<Id>();
		}

		template<size_t Id>
		auto const &getSystem() const noexcept {
			static_assert(Id < Specifications::size, ECS_SPEC_OUT_OF_BOUNDS);
			return this->system<Id>();
		}

		template<typename Spec, typename ...Args>
		auto &construct(Spec const &spec, Args&&... args) noexcept(noexcept(std::declval<SystemOf<Spec>>(std::declval<Args>()...)))
		{
			static_assert(isOwnSpec<Spec>, ECS_UNKNOWN_SPEC);
			auto &sys = this->system<Spec>();
			
			new (std::addressof(sys)) SystemOf<Spec>(spec(std::forward<Args>(args)...));
			this->constructed.set(specId<Spec>);
			return sys;
		}

		Manager &manager;
		std::bitset<sizeof...(Specs)> constructed;
		std::tuple<SystemStorage<Specs>...> systems;

	private:
		template<typename T>
		constexpr auto &system() noexcept { return reinterpret_cast<SystemOf<T> &>(std::get<specId<T>>(this->systems));}
		template<typename T>
		constexpr auto const &system() const noexcept { return reinterpret_cast<SystemOf<T> const &>(std::get<specId<T>>(this->systems));}
		template<size_t Id>
		constexpr auto &system() noexcept { return reinterpret_cast<SystemById<Id> &>(std::get<Id>(this->systems));}
		template<size_t Id>
		constexpr auto const &system() const noexcept { return reinterpret_cast<SystemById<Id> const &>(std::get<Id>(this->systems));}
	};

	template<typename TSettings, typename ...Specs>
	Machinery(ecs::Manager<TSettings>, Specs const &...) -> Machinery<TSettings, Specs...>;

	// template<typename TSettings, typename ...Specs>
	// Machinery(ecs::Manager<TSettings>, Specs...) -> Machinery<TSettings, Specs...>;
} // ecs

#undef ECS_UNKNOWN_SPEC
#undef ECS_SPEC_OUT_OF_BOUNDS