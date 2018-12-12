#pragma once
#include "el/static_if.hpp"
#include "el/conditional.hpp"
#include "el/is_same.hpp"
#include "el/enable_if.hpp"
#include "el/types/is_valid.hpp"

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

		template<typename T>
		struct SystemImage {
			T t;
			constexpr operator T &() noexcept { return t; }
			constexpr operator T const &() const noexcept { return t; }
		};
		template<>
		struct SystemImage<void> {};
	} // impl

	// template<typename T>
	// using SystemImage = typename ecs::impl::SystemImage<T>::Type;
	template<typename T>
	using SystemImage = el::conditional_t<
		std::is_class<T>::value,
		T,
		ecs::impl::SystemImage<T>
	>;
	template<typename FCallback, typename TDependencyList, typename TInstance, typename TSignature>
	class System;

	template<typename FCallback, typename TDependencyList, typename TSignature>
	class System<FCallback, TDependencyList, void, TSignature>: public std::decay_t<FCallback> {
	public:
		using This = System<FCallback, TDependencyList, void, TSignature>;
		using Callback = typename std::decay<FCallback>::type;
		using Dependencies = TDependencyList;
		using Signature = TSignature;
		using Image = void;

		constexpr static bool hasImage = false;

		System(This const &oth): Callback(dynamic_cast<Callback&>(oth))
		{
		}

		System(This &&oth): Callback(std::move(dynamic_cast<Callback&&>(oth)))
		{
		}

		template<typename Function>
		System(Function &&f): Callback(std::forward<Function>(f))
		{
		}

		template<typename TSettings, typename ...Args>
		void operator()(ecs::Manager<TSettings> &mgr, Args&&... args)
		{
			Signature &sig = this->_sig;
			Callback &callback = *this;

			mgr.forEntitiesMatching(sig, callback, std::forward<Args>(args)...);
		}

	private:
		Signature _sig;
	};

	template<typename FCallback, typename TDependencyList, typename TInstance, typename TSignature>
	class System:
		public std::decay_t<FCallback>,
		// public SystemImage<TInstance>,
		// public el::conditional_t<
		// 	std::is_class<TInstance>::value,
		// 	TInstance,
		// 	ecs::impl::SystemImage<TInstance>
		// > {
		public ecs::SystemImage<TInstance> {
	public:
		using This = System<FCallback, TDependencyList, TInstance, TSignature>;
		using Callback = typename std::decay_t<FCallback>;
		using Dependencies = TDependencyList;
		using Image = ecs::SystemImage<TInstance>;
		using Signature = TSignature;

		constexpr static bool hasImage = true;

		System(This const &oth):
		Callback(dynamic_cast<Callback&>(oth)), Image(oth.image())
		{
		}

		System(This &&oth):
		Callback(std::move(dynamic_cast<Callback&&>(oth))), Image(std::move(oth.image()))
		{
		}

		template<typename Function, typename ...ImageArgs>
		System(Function &&f, ImageArgs&&... args):
		Callback(std::forward<Function>(f)), Image(std::forward<ImageArgs>(args)...)
		{
		}

		constexpr explicit operator Image&() noexcept { return *this; }
		constexpr explicit operator Image const &() const noexcept { return *this; }

		constexpr Image &image() noexcept { return *this; }
		constexpr Image const &image() const noexcept { return *this; }

		template<typename TSettings, typename ...Args>
		void operator()(ecs::Manager<TSettings> &mgr, Args&&... args)
		{
			Signature &sig = this->_sig;
			Callback &callback = *this;
			Image &img = *this;

			mgr.forEntitiesMatching(sig, callback, img, std::forward<Args>(args)...);
		}

	private:
		Signature _sig;
	};

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
		using Callback = typename std::decay_t<FCallback>;

		Callback callback;

		constexpr explicit SystemSpecs(Callback &f): callback(f)
		{
		}

		constexpr explicit SystemSpecs(Callback const &f): callback(f)
		{
		}

		constexpr explicit SystemSpecs(Callback &&f): callback(std::move(f))
		{
		}

		constexpr explicit SystemSpecs(Callback const &&f): callback(std::move(f))
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
			return SystemSpecs<typename std::decay<F>::type, Dependencies, Instance, Signature>(std::forward<F>(f));
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