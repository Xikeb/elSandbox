#pragma once

#include <type_traits>
#include <functional>

#include "el/type_list/type_list.hpp"
#include "el/remove_cv.hpp"
#include "Signature.hpp"
#include "System.hpp"

namespace ecs {
	namespace impl {
		struct SystemSettings {
			struct Dependencies;
			struct Instance;
			struct Signature;
			struct Callback;

			constexpr static auto &dependency = el::type_c<Dependencies>;
			constexpr static auto &instance = el::type_c<Instance>;
			constexpr static auto &signature = el::type_c<Signature>;
			constexpr static auto &callback = el::type_c<Callback>;
			struct Keys {
				using Dependencies = el::remove_cvref_t<decltype(dependency)>;
				using Instance = el::remove_cvref_t<decltype(instance)>;
				using Signature = el::remove_cvref_t<decltype(signature)>;
				using Callback = el::remove_cvref_t<decltype(callback)>;
			};
		};
	} // impl

	template<typename FCallback = void(void),
		typename TDependencyList = el::type_list<>,
		typename TInstance = void,
		typename TSignature = void>
	class SystemSpecs {
	public:
		using Settings = ecs::impl::SystemSettings;
		using Dependencies = TDependencyList;
		using Instance = TInstance;
		using Signature = TSignature;
		using Callback = FCallback;
		using System = ecs::System<
			el::conditional_t<
				!std::is_class<Callback>::value,
				std::function<
					std::remove_pointer_t<std::decay_t<Callback>>
				>,
				Callback
			>,
			Dependencies,
			el::conditional_t<
				std::is_void<Instance>::value,
				void,
				Instance
			>,
			el::conditional_t<
				std::is_void<Signature>::value,
				void,
				Signature
			>
		>;

		Callback callback;

		constexpr explicit SystemSpecs(Callback const &f): callback(f)
		{
		}

		constexpr explicit SystemSpecs(el::remove_ref_t<Callback> &&f): callback(std::move(f))
		/*options(
			el::make_pair(Settings::dependency, el::type_c<Dependencies>),
			el::make_pair(Settings::instance, el::type_c<Instance>),
			el::make_pair(Settings::signature, el::type_c<Signature>),
			el::make_pair(Settings::callback, std::forward<Callback>(f))
		)*/
		{
		}

		template<typename T>
		constexpr auto instantiateWith(el::Type_c<T>) const noexcept
		{
			return ecs::SystemSpecs<Callback, Dependencies, T, Signature>(this->callback);
		}

		template<typename T>
		constexpr auto instantiateWith() const noexcept
		{
			return ecs::SystemSpecs<Callback, Dependencies, T, Signature>(this->callback);
		}

		template<typename F>
		constexpr auto execution(F&& f) const noexcept
		{
			return ecs::SystemSpecs<F, Dependencies, Instance, Signature>(std::forward<F>(f));
		}

		template<typename ...Requirements>
		constexpr auto after(el::Type_c<el::type_list<Requirements...>>) const noexcept
		{
			return ecs::SystemSpecs<Callback, typename Dependencies::template Push<Requirements...>, Instance, Signature>(this->callback);
		}

		template<typename TNewSignature>
		constexpr auto matching(el::Type_c<TNewSignature>) const noexcept
		{
			return ecs::SystemSpecs<Callback, Dependencies, Instance, TNewSignature>(this->callback);
		}

		template<typename TNewSignature>
		constexpr auto matching() const noexcept
		{
			return ecs::SystemSpecs<Callback, Dependencies, Instance, TNewSignature>(this->callback);
		}

		constexpr auto manual() const noexcept
		{
			return ecs::SystemSpecs<Callback, Dependencies, Instance, void>(this->callback);
		}

		template<typename ...Args>
		constexpr auto operator()(Args&&... args) const noexcept
		{
			return System(this->callback, std::forward<Args>(args)...);
		}
	};

	template<typename FCallback>
	constexpr auto makeSystem(FCallback &&f) noexcept {
		return ecs::SystemSpecs<FCallback>(std::forward<FCallback>(f));
	}

	namespace detail {
		template<typename T>
		constexpr static bool is_system_specs = el::is_similar_v<ecs::SystemSpecs, T>;
	} // detail
} // ecs