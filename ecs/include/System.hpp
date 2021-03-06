#pragma once

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <memory>
#include <utility>
#include <type_traits>

#include "el/static_if.hpp"
#include "el/conditional.hpp"
#include "el/is_same.hpp"
#include "el/enable_if.hpp"
#include "el/types/is_valid.hpp"

#include "Manager.hpp"

namespace ecs {
	namespace impl {
		template<typename T>
		struct SystemImage {
			using ValueType = T;
			using Storage = el::conditional_t<
				std::is_scalar<T>::value,
				ValueType,
				std::aligned_storage_t<sizeof(ValueType), alignof(ValueType)>
			>;

			SystemImage(ValueType const &v)
			{
				this->construct(v);
			}

			SystemImage(ValueType &&v)
			{
				this->construct(std::move(v));
			}

			template<typename ...Args>
			SystemImage(Args&&... args)
			{
				this->construct(std::forward<Args>(args)...);
			}

			constexpr operator ValueType &() 		noexcept 		{ return reinterpret_cast<T&>(this->storage); }
			constexpr operator ValueType const &() 	const noexcept 	{ return reinterpret_cast<T const &>(this->storage); }

			template<typename ...Args>
			void construct(Args&&... args) noexcept(noexcept(T(std::declval<Args>()...)))
			{
				new (static_cast<void *>(std::addressof(this->storage))) T(std::forward<Args>(args)...);
			}

			Storage storage;
		};

		template<>
		struct SystemImage<void> {
			using ValueType = void;
			using Storage = void;
		};
	} // impl

	// Any Image class might have several constructors, and default might be only one of them,
	// and any image scalar might be const/volatile,
	// So explicit construction should be available for any image
	// Conversely, it might be good to make it possible to construct explicitly the image
	// in the system's constructor by use of a meta-tag el::type_c<ecs::construct_image>,
	// followed by variadic arguments
	template<typename T>
	using SystemImage = ecs::impl::SystemImage<T>;

	template<typename FCallback, typename TDependencyList, typename TInstance, typename TSignature>
	class System;

	/*
	 * Neither Image nor Signature
	*/
	template<typename FCallback, typename TDependencyList>
	class System<FCallback, TDependencyList, void, void>: private FCallback {
	public:
		using Self = System<FCallback, TDependencyList, void, void>;
		using Callback = FCallback;
		using Dependencies = TDependencyList;
		using Signature = void;
		using Image = void;

		constexpr static bool hasImage = false;

		System(Self const &oth): FCallback(dynamic_cast<Callback&>(oth))
		{
		}

		System(Self &&oth): FCallback(std::move(dynamic_cast<Callback&&>(oth)))
		{
		}

		template<typename Function>
		System(Function &&f): FCallback(std::forward<Function>(f))
		{
		}

		template<typename ...Args>
		void constructImage(Args&&...) const noexcept
		{
			static_assert(sizeof...(Args) && !sizeof...(Args), "No Image to construct.");
		}

		template<typename TSettings, typename ...Args>
		void operator()(ecs::Manager<TSettings> &mgr, Args&&... args)
		{
			Callback &callback = *this;

			callback(mgr, std::forward<Args>(args)...);
		}
	};


	/*
	 * available Signature, void Image
	*/
	template<typename FCallback, typename TDependencyList, typename TSignature>
	class System<FCallback, TDependencyList, void, TSignature>: private FCallback {
	public:
		using Self = System<FCallback, TDependencyList, void, TSignature>;
		using Callback = FCallback;
		using Dependencies = TDependencyList;
		using Signature = TSignature;
		using Image = void;

		constexpr static bool hasImage = false;

		System(Self const &oth): FCallback(dynamic_cast<Callback&>(oth))
		{
		}

		System(Self &&oth): FCallback(std::move(dynamic_cast<Callback&&>(oth)))
		{
		}

		template<typename Function>
		System(Function &&f): FCallback(std::forward<Function>(f))
		{
		}

		template<typename ...Args>
		void constructImage(Args&&...) const noexcept
		{
			static_assert(sizeof...(Args) && !sizeof...(Args), "No Image to construct.");
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

	/*
	 * void Signature, available Image
	*/

	template<typename FCallback, typename TDependencyList, typename TInstance>
	class System<FCallback, TDependencyList, TInstance, void>:
		private FCallback,
		public ecs::SystemImage<TInstance> {
	public:
		using Self = System<FCallback, TDependencyList, TInstance, void>;
		using Callback = FCallback;
		using Dependencies = TDependencyList;
		using Image = TInstance;
		using Signature = void;

	private: using ImageContainer = ecs::impl::SystemImage<TInstance>;
	public:

		constexpr static bool hasImage = true;

		System(Self const &oth):
			FCallback(dynamic_cast<Callback&>(oth)),
			ImageContainer(oth.image())
		{
		}

		System(Self &&oth):
			FCallback(std::move(dynamic_cast<Callback&&>(oth))),
			ImageContainer(std::move(oth.image()))
		{
		}

		template<typename Function, typename ...ImageArgs>
		System(Function &&f, ImageArgs&&... args):
			FCallback(std::forward<Function>(f)),
			ImageContainer(std::forward<ImageArgs>(args)...)
		{
		}

		constexpr operator Image&() noexcept 			{ assert(this->constructed); return *static_cast<ImageContainer *>(this); }
		constexpr operator Image const &() const noexcept	{ assert(this->constructed); return *static_cast<ImageContainer *>(this); }

		constexpr Image &image() noexcept 			{ assert(this->constructed); return *static_cast<ImageContainer *>(this); }
		constexpr Image const &image() const noexcept 		{ assert(this->constructed); return *static_cast<ImageContainer *>(this); }

		template<typename ...Args>
		Image &constructImage(Args&&... args) noexcept(noexcept(std::declval<ImageContainer>().construct(std::declval<Args>()...)))
		{
			static_cast<ImageContainer *>(this)->construct(std::forward<Args>(args)...);
		}

		template<typename TSettings, typename ...Args>
		void operator()(ecs::Manager<TSettings> &mgr, Args&&... args)
		{
			Callback &callback = *this;

			callback(
				mgr,
				this->image(),
				std::forward<Args>(args)...
			);
		}
	#ifndef NDEBUG
	private: bool constructed = true;
	public:
	#endif // NDEBUG
	};

	/*
	 * available Signature, available Image
	*/
	template<typename FCallback, typename TDependencyList, typename TInstance, typename TSignature>
	class System:
		private FCallback,
		public ecs::SystemImage<TInstance> {
	public:
		using Self = ecs::System<FCallback, TDependencyList, TInstance, TSignature>;
		using Callback = FCallback;
		using Dependencies = TDependencyList;
		using Image = TInstance;
		using Signature = TSignature;

	private: using ImageContainer = ecs::impl::SystemImage<TInstance>;
	public:

		constexpr static bool hasImage = true;

		System(Self const &oth):
			FCallback(dynamic_cast<Callback&>(oth)),
			ImageContainer(oth.image())
		{
		}

		System(Self &&oth):
			FCallback(std::move(dynamic_cast<Callback&&>(oth))),
			ImageContainer(std::move(oth.image()))
		{
		}

		template<typename Function, typename ...ImageArgs>
		System(Function &&f, ImageArgs&&... args):
			FCallback(std::forward<Function>(f)),
			ImageContainer(std::forward<ImageArgs>(args)...)
		{
		}

		constexpr operator Image&() noexcept 			{ assert(this->constructed); return *static_cast<ImageContainer *>(this); }
		constexpr operator Image const &() const noexcept	{ assert(this->constructed); return *static_cast<ImageContainer *>(this); }

		constexpr Image &image() noexcept 			{ assert(this->constructed); return *static_cast<ImageContainer *>(this); }
		constexpr Image const &image() const noexcept 		{ assert(this->constructed); return *static_cast<ImageContainer *>(this); }

		template<typename ...Args>
		Image &constructImage(Args&&... args) noexcept(noexcept(std::declval<ImageContainer>().construct(std::declval<Args>()...)))
		{
			static_cast<ImageContainer *>(this)->construct(std::forward<Args>(args)...);
		}

		template<typename TSettings, typename ...Args>
		void operator()(ecs::Manager<TSettings> &mgr, Args&&... args)
		{
			mgr.forEntitiesMatching(
				this->_sig,
				*static_cast<Callback *>(this),
				this->image(),
				std::forward<Args>(args)...
			);
		}

	private:
		Signature _sig;
	#ifndef NDEBUG
		bool constructed = true;
	#endif // NDEBUG
	};
} // ecs