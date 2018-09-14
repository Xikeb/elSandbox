#ifndef ELELCS_SYSTEM_HPP
	#define ELELCS_SYSTEM_HPP
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

			struct Empty {};

			template<typename T>
			struct SystemImage {
			private:
				struct Contain {
					T t;
					constexpr operator T &() noexcept { return t; }
					constexpr operator T const &() const noexcept { return t; }
				};

				struct Extend: public T {};
			public:
				using Type = el::conditional_t<
					std::is_class<T>::value,
					Extend, Contain
				>;
			};
		} // impl

		template<typename T>
		using SystemImage = typename ecs::impl::SystemImage<T>::Type;

		template<typename FCallback, typename TDependencyList, typename TInstance, typename TSignature>
		class System:
			public std::decay<FCallback>::type,
			public SystemImage<TInstance> {
		public:
			using This = System<FCallback, TDependencyList, TInstance, TSignature>;
			using Callback = typename std::decay<FCallback>::type;
			using Dependencies = TDependencyList;
			using Image = SystemImage<TInstance>;
			using Signature = TSignature;

			const bool hasImage = true;

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

			constexpr explicit operator Image&() noexcept
			{
				return *this;
			}

			constexpr Image &image() noexcept { return *this; }
			constexpr Image const &image() const noexcept { return *this; }

			template<typename TSettings, typename ...Args>
			void operator()(ecs::Manager<TSettings> &mgr, Args&&... args)
			{
				Callback &callback = *this;
				Image &img = *this;

				mgr.forEntitiesMatching(this->_sig, callback, img, std::forward<Args>(args)...);
			}

		private:
			Signature _sig;
		};

		template<typename FCallback, typename TDependencyList, typename TSignature>
		class System<FCallback, TDependencyList, void, TSignature>: public std::decay<FCallback>::type {
		public:
			using This = System<FCallback, TDependencyList, void, TSignature>;
			using Callback = typename std::decay<FCallback>::type;
			using Dependencies = TDependencyList;
			using Signature = TSignature;
			using Image = void;

			const bool hasImage = false;

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
				Callback &callback = *this;

				mgr.forEntitiesMatching(this->_sig, callback, std::forward<Args>(args)...);
			}

		private:
			Signature _sig;
		};

		template<typename FCallback = void(void), typename TDependencyList = el::type_list<>, typename TInstance = ecs::impl::Empty, typename TSignature = ecs::SignatureTrue>
		class SystemSetup {
		public:
			using Settings = ecs::impl::SystemSettings;
			using Dependencies = TDependencyList;
			using Instance = TInstance;
			using Signature = TSignature;
			using Callback = typename std::decay<FCallback>::type;

			/*el::map<
				el::pair<typename Settings::Keys::Dependencies, el::Type_c<Dependencies>>,
				el::pair<typename Settings::Keys::Instance, el::Type_c<Instance>>,
				el::pair<typename Settings::Keys::Signature, el::Type_c<Signature>>,
				el::pair<typename Settings::Keys::Callback, Callback>
			> options;*/
			Callback callback;

			constexpr explicit SystemSetup(Callback &f): callback(f)
			{
			}

			constexpr explicit SystemSetup(Callback const &f): callback(f)
			{
			}

			constexpr explicit SystemSetup(Callback &&f): callback(std::move(f))
			{
			}

			constexpr explicit SystemSetup(Callback const &&f): callback(std::move(f))
			/*options(
				el::make_pair(Settings::dependency, el::type_c<Dependencies>),
				el::make_pair(Settings::instance, el::type_c<Instance>),
				el::make_pair(Settings::signature, el::type_c<Signature>),
				el::make_pair(Settings::callback, std::forward<Callback>(f))
			)*/
			{
			}

			template<typename T>
			constexpr auto instantiateWith(el::Type_c<T>) noexcept
			{
				return SystemSetup<Callback, Dependencies, T, Signature>(this->callback);
			}

			template<typename T>
			constexpr auto instantiateWith() noexcept
			{
				return SystemSetup<Callback, Dependencies, T, Signature>(this->callback);
			}

			template<typename F>
			constexpr auto execution(F&& f) noexcept
			{
				return SystemSetup<typename std::decay<F>::type, Dependencies, Instance, Signature>(std::forward<F>(f));
			}

			template<typename ...Requirements>
			constexpr auto after(el::Type_c<el::type_list<Requirements...>>) noexcept
			{
				return SystemSetup<Callback, el::type_list<Requirements...>, Instance, Signature>(this->callback);
			}

			template<typename TNewSignature>
			constexpr auto matching(el::Type_c<TNewSignature>) noexcept
			{
				return SystemSetup<Callback, Dependencies, Instance, TNewSignature>(this->callback);
			}

			template<typename ...Args>
			constexpr auto operator()(Args&&... args) noexcept
			{
				return System<Callback, Dependencies, Instance, Signature>(
					this->callback, std::forward<Args>(args)...
				);
			}
		};

		template<typename FCallback>
		constexpr auto makeSystem(FCallback &&f) noexcept {
			return SystemSetup<FCallback>(std::forward<FCallback>(f));
		}
	
	} // ecs

#endif // ELELCS_SYSTEM_HPP
