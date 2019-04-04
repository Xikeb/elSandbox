#pragma once
#include <tuple>
#include <functional>
#include <type_traits>

#include "el/is_same.hpp"
#include "el/types/type_c.hpp"
#include "el/type_list/type_list.hpp"

#include "Settings.hpp"
#include "systemSpecs.hpp"
#include "System.hpp"
#include "Manager.hpp"

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

		using AutomaticSystems = Specifications::filter([](auto &&spec){
			return !el::is_same<
				typename TYPE_OF(+spec)::System::Signature,
				void
			>{};
		});

		template<typename Spec>
		constexpr static size_t systemId = Specifications::template IndexOf<Spec>::value;

		static_assert(
			ecs::detail::is_settings<Settings>,
			"Machinery requires ecs::Settings to be properly constructed."
		);
		Specifications::for_each([](auto &&specs, auto&&){
			static_assert(
				ecs::detail::is_system_specs<TYPE_OF(specs)>,
				"Each Specification must be an instance of the ecs::SystemSpecs template."
			);
		});

		Machinery(Manager const &mgr): manager(mgr), constructed(0)
		{
		}

		template<typename Spec>
		auto &getSystem(Spec) noexcept
		{
			static_assert(
				Specifications::template Contains<Spec>::value,
				"Those specifications do not represent a System of this Machine."
			);
			return reinterpret_cast<typename Spec::System &>(
				std::get< systemId<Spec> >(this->systems)
			);
		}

		template<typename Spec>
		auto const &getSystem(Spec) const noexcept
		{
			static_assert(
				Specifications::template Contains<Spec>::value,
				"Those specifications do not represent a System of this Machine."
			);
			return reinterpret_cast<typename Spec::System const &>(
				std::get< systemId<Spec> >(this->systems)
			);
		}

		template<typename Spec>
		auto &getSystem() noexcept
		{
			static_assert(
				Specifications::template Contains<Spec>::value,
				"Those specifications do not represent a System of this Machine."
			);
			return reinterpret_cast<typename Spec::System &>(
				std::get< systemId<Spec> >(this->systems)
			);
		}

		template<typename Spec>
		auto &getSystem() const noexcept
		{
			static_assert(
				Specifications::template Contains<Spec>::value,
				"Those specifications do not represent a System of this Machine."
			);
			return reinterpret_cast<typename Spec::System const &>(
				std::get< systemId<Spec> >(this->systems)
			);
		}

		template<typename Spec, typename ...Args>
		auto &construct(Spec, Args&&... args) noexcept(noexcept(std::declval<typename Spec::System>(std::declval<Args>()...)))
		{
			static_assert(
				Specifications::template Contains<Spec>::value,
				"Those specifications do not represent a System of this Machine."
			);
			this->constructed.set(systemId<Spec>);
			auto &sys = reinterpret_cast<typename Spec::System &>(
				std::get< systemId<Spec> >(this->systems)
			);
			new (std::addressof(sys)) typename Spec::System(std::forward<Args>(args)...);
			return sys;
		}

		template<typename Spec, typename ...Args>
		auto &construct(Args&&... args) noexcept(noexcept(std::declval<typename Spec::System>(std::declval<Args>()...)))
		{
			static_assert(
				Specifications::template Contains<Spec>::value,
				"Those specifications do not represent a System of this Machine."
			);
			this->constructed.set(systemId<Spec>);
			auto &sys = reinterpret_cast<typename Spec::System &>(
				std::get< systemId<Spec> >(this->systems)
			);
			new (std::addressof(sys)) typename Spec::System(std::forward<Args>(args)...);
			return sys;
		}

		Manager &manager;
		std::tuple<SystemStorage<Specs>...> systems;
		std::bitset<sizeof...(Specs)> constructed;
	};

	template<typename TSettings, typename ...Specs>
	ecs::Machinery(ecs::Manager<TSettings>, Specs const &...) -> ecs::Machinery<TSettings, Specs...>;

	template<typename TSettings, typename ...Specs>
	ecs::Machinery(ecs::Manager<TSettings>, Specs...) -> ecs::Machinery<TSettings, Specs...>;
} // ecs