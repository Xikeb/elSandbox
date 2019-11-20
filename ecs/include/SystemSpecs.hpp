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

		constexpr explicit SystemSpecs(Callback const &f) noexcept: callback(f)
		{
		}

		constexpr explicit SystemSpecs(el::remove_ref_t<Callback> &&f) noexcept: callback(std::move(f))
		{
		}

		template<typename Image>
		constexpr auto instantiateWith(el::Type_c<Image>) const noexcept {
			return ecs::SystemSpecs<
				Callback,
				Dependencies,
				Image,
				Signature
			>(this->callback);
		}

		template<typename Image>
		constexpr auto instantiateWith() const noexcept {
			return ecs::SystemSpecs<
				Callback,
				Dependencies,
				Image,
				Signature
			>(this->callback);
		}

		template<typename F>
		constexpr auto execution(F&& f) const noexcept {
			return ecs::SystemSpecs<F, Dependencies, Instance, Signature>(std::forward<F>(f));
		}

		template<typename ...Requirements>
		constexpr auto after(el::Type_c<el::type_list<Requirements...>>) const noexcept {
			return ecs::SystemSpecs<
				Callback,
				typename Dependencies::template Push<Requirements...>,
				Instance,
				Signature
			>(this->callback);
		}

		template<typename TSig>
		constexpr auto matching(el::Type_c<TSig>) const noexcept {
			return ecs::SystemSpecs<Callback, Dependencies, Instance, TSig>(this->callback);
		}

		template<typename TSig>
		constexpr auto matching() const noexcept {
			return ecs::SystemSpecs<Callback, Dependencies, Instance, TSig>(this->callback);
		}

		constexpr auto manual() const noexcept { //Write entire system execution yourself
			return ecs::SystemSpecs<Callback, Dependencies, Instance, void>(this->callback);
		}

		template<typename ...Args>
		constexpr auto system(Args&&... args) const noexcept {
			return System(this->callback, std::forward<Args>(args)...);
		}

		template<typename ...Args>
		constexpr auto operator()(Args&&... args) const noexcept {
			return this->system(std::forward<Args>(args)...);
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