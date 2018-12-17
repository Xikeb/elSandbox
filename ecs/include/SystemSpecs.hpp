#pragma once
#include "el/type_list/type_list.hpp"
#include "el/remove_cv.hpp"
#include "Signature.hpp"

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
		typename TSignature = ecs::SignatureTrue>
	class SystemSpecs {
	public:
		using Settings = ecs::impl::SystemSettings;
		using Dependencies = TDependencyList;
		using Instance = TInstance;
		using Signature = TSignature;
		using Callback = std::decay_t<FCallback>;

		Callback callback;

		constexpr explicit SystemSpecs(Callback const &f): callback(f)
		{
		}

		constexpr explicit SystemSpecs(Callback &&f): callback(std::move(f))
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
			return SystemSpecs<Callback, Dependencies, T, Signature>(this->callback);
		}

		template<typename T>
		constexpr auto instantiateWith() const noexcept
		{
			return SystemSpecs<Callback, Dependencies, T, Signature>(this->callback);
		}

		template<typename F>
		constexpr auto execution(F&& f) const noexcept
		{
			return SystemSpecs<std::decay_t<F>, Dependencies, Instance, Signature>(std::forward<F>(f));
		}

		template<typename ...Requirements>
		constexpr auto after(el::Type_c<el::type_list<Requirements...>>) const noexcept
		{
			return SystemSpecs<Callback, el::type_list<Requirements...>, Instance, Signature>(this->callback);
		}

		template<typename TNewSignature>
		constexpr auto matching(el::Type_c<TNewSignature>) const noexcept
		{
			return SystemSpecs<Callback, Dependencies, Instance, TNewSignature>(this->callback);
		}

		template<typename ...Args>
		constexpr auto operator()(Args&&... args) const noexcept
		{
			return System<Callback, Dependencies, Instance, Signature>(
				this->callback, std::forward<Args>(args)...
			);
		}
	};

	template<typename FCallback>
	constexpr auto makeSystem(FCallback &&f) noexcept {
		return SystemSpecs<FCallback>(std::forward<FCallback>(f));
	}
} // ecs