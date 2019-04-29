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

#define ECS_UNKNOWN_SPEC	"Those specifications do not represent a System of this Machine."

namespace ecs {
	template<typename TSettings, typename ...Specs>
	struct Machinery {
		using Self = ecs::Machinery<TSettings, Specs...>;
		using Settings = TSettings;
		using Manager = ecs::Manager<Settings>;
		using Specifications = el::type_list<Specs...>;

		template<typename Spec>
		using SystemStorage = std::aligned_storage_t<
			sizeof(typename Spec::System),
			alignof(typename Spec::System)
		>;

		struct is_not_void {
			template<typename T>
			constexpr static auto value = !el::is_same<
				typename T::System::Signature,
				void
			>{};

			template<typename T>
			constexpr auto operator()() const noexcept { return value<T>; }

			template<typename T>
			constexpr auto operator()(el::Type_c<T>) const noexcept { return value<T>; }
		};

		using AutomaticSystems = typename Specifications::template Filter<is_not_void>;

		template<typename Spec>
		constexpr static size_t systemId = Specifications::template IndexOf<Spec>::value;
		template<typename Spec>
		constexpr static bool isOwnSpec = Specifications::template Contains<Spec>::value;

		template<size_t Id>
		using SpecById = typename Specifications::template At<Id>;

		static_assert(
			ecs::detail::is_settings<Settings>,
			"Machinery requires ecs::Settings to be properly constructed."
		);
		constexpr static auto isOnlySpecs = Specifications::for_each([](auto &&specs, auto&&){
			static_assert(
				ecs::detail::is_system_specs<TYPE_OF(specs)>,
				"Each Specification must be an instance of the ecs::SystemSpecs template."
			);
		});

		constexpr Machinery(Manager &mgr) noexcept:
		manager(mgr), constructed(0)
		{
		}

		constexpr Machinery(Manager &mgr, Specs const &...) noexcept:
		manager(mgr), constructed(0)
		{
		}

		template<typename Spec>
		auto &getSystem(Spec) noexcept
		{
			static_assert(isOwnSpec<Spec>, ECS_UNKNOWN_SPEC);
			return reinterpret_cast<typename Spec::System &>(
				std::get< systemId<Spec> >(this->systems)
			);
		}

		template<typename Spec>
		auto const &getSystem(Spec) const noexcept
		{
			static_assert(isOwnSpec<Spec>, ECS_UNKNOWN_SPEC);
			return reinterpret_cast<typename Spec::System const &>(
				std::get< systemId<Spec> >(this->systems)
			);
		}

		template<typename Spec>
		auto &getSystem() noexcept
		{
			static_assert(isOwnSpec<Spec>, ECS_UNKNOWN_SPEC);
			return reinterpret_cast<typename Spec::System &>(
				std::get< systemId<Spec> >(this->systems)
			);
		}

		template<typename Spec>
		auto &getSystem() const noexcept
		{
			static_assert(isOwnSpec<Spec>, ECS_UNKNOWN_SPEC);
			return reinterpret_cast<typename Spec::System const &>(
				std::get< systemId<Spec> >(this->systems)
			);
		}

		template<size_t Id>
		auto &getSystem() noexcept
		{
			static_assert(
				Id < Specifications::size,
				"System index out of bounds."
			);

			return reinterpret_cast<typename SpecById<Id>::System &>(
				std::get<Id>(this->systems)
			);
		}

		template<size_t Id>
		auto &getSystem() const noexcept
		{
			static_assert(
				Id < Specifications::size,
				"System index out of bounds."
			);

			return reinterpret_cast<typename SpecById<Id>::System const &>(
				std::get<Id>(this->systems)
			);
		}

		template<typename Spec, typename ...Args>
		auto &construct(Spec const &spec, Args&&... args) noexcept(noexcept(std::declval<typename Spec::System>(std::declval<Args>()...)))
		{
			static_assert(isOwnSpec<Spec>, ECS_UNKNOWN_SPEC);
			auto &sys = reinterpret_cast<typename Spec::System &>(
				std::get< systemId<Spec> >(this->systems)
			);
			
			new (std::addressof(sys)) typename Spec::System(spec(std::forward<Args>(args)...));
			this->constructed.set(systemId<Spec>);
			return sys;
		}

		Manager &manager;
		std::bitset<sizeof...(Specs)> constructed;
		std::tuple<SystemStorage<Specs>...> systems;
	};

	template<typename TSettings, typename ...Specs>
	Machinery(ecs::Manager<TSettings>, Specs const &...) -> Machinery<TSettings, Specs...>;

	// template<typename TSettings, typename ...Specs>
	// Machinery(ecs::Manager<TSettings>, Specs...) -> Machinery<TSettings, Specs...>;
} // ecs