#ifndef ELELCS_SYSTEM_HPP
	#define ELELCS_SYSTEM_HPP
	#include "el/static_if.hpp"
	#include "el/conditional.hpp"
	#include "el/is_same.hpp"
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
		} // impl

		template<typename FCallback,/* typename TSettings,*/ typename TDependencyList, typename TInstance, typename TSignature>
		class System {
		public:
			using This = System<FCallback,/* TSettings,*/ TDependencyList, TInstance, TSignature>;
			using Dependencies = TDependencyList;
			using Instance = typename el::conditional<
					el::is_same<TInstance, void>::value,
					ecs::impl::Empty,
					TInstance
				>::type;
			using Signature = TSignature;
			using Callback = FCallback;
			// using Manager = ecs::Manager<TSettings>;

			System(This const &oth):
			image(oth.image), _f(oth._f)
			{
			}

			System(This &&oth):
			image(std::move(oth.image)), _f(std::move(oth._f))
			{
			}

			template<typename ...Args>
			System(/*std::function<Callback> f*/Callback &&f, Args&&... args):
			image(std::forward<Args>(args)...), _f(std::move(f))
			{
			}

			/*
			System(Manager &mgr, std::function<Callback> &&f, Instance &&i):
			image(std::move(i)), _mgr(mgr), _f(std::move(f))
			{
			}

			System(Manager &mgr, std::function<Callback> &&f, Instance const &i):
			image(i), _mgr(mgr), _f(std::move(f))
			{
			}
			*/

			template<typename TSettings, typename ...Args>
			void operator()(ecs::Manager<TSettings> &mgr, Args&&... args)
			{
				static const auto ifHasImage = el::static_if<!el::is_same<TInstance, void>::value>();
				static const auto ifHasSignature = el::static_if<el::is_same<Signature, el::type_list<>>::value>();

				static_assert(!ifHasSignature, "No Signature?");
				static_assert(ifHasImage, "No Image?");
				ifHasSignature
					.then([&](){
						// ifHasImage
						// 	.then([&](){
						// 		mgr.forEntitiesMatching(this->_sig, this->_f, this->image, std::forward<Args>(args)...);
						// 	})
						// 	.otherwise([&](){
						// 		mgr.forEntitiesMatching(this->_sig, this->_f, std::forward<Args>(args)...);
						// 	});
						if (ifHasImage)
							mgr.forEntitiesMatching(this->_sig, this->_f, this->image, std::forward<Args>(args)...);
						else
							mgr.forEntitiesMatching(this->_sig, this->_f, std::forward<Args>(args)...);
					})
					.otherwise([&](){
						// ifHasImage
						// 	.then([&](){
						// 		mgr.forEntities(this->_f, this->image, std::forward<Args>(args)...);
						// 	})
						// 	.otherwise([&](){
						// 		mgr.forEntities(this->_f, std::forward<Args>(args)...);
						// 	});
						if (ifHasImage)
							mgr.forEntities(this->_f, this->image, std::forward<Args>(args)...);
						else
							mgr.forEntities(this->_f, std::forward<Args>(args)...);
					});
			}


			Instance image;
			const bool hasImage = !el::is_same<TInstance, void>::value;
		private:
			//Manager &_mgr;
			// std::function<Callback> _f;
			Callback _f;
			Signature _sig;
		};

		template<typename FCallback = void(void), typename TDependencyList = el::type_list<>, typename TInstance = ecs::impl::Empty, typename TSignature = el::type_list<>>
		class SystemSetup {
		public:
			using Settings = ecs::impl::SystemSettings;
			using Dependencies = TDependencyList;
			using Instance = TInstance;
			using Signature = TSignature;
			using Callback = FCallback;

			/*el::map<
				el::pair<typename Settings::Keys::Dependencies, el::Type_c<Dependencies>>,
				el::pair<typename Settings::Keys::Instance, el::Type_c<Instance>>,
				el::pair<typename Settings::Keys::Signature, el::Type_c<Signature>>,
				el::pair<typename Settings::Keys::Callback, Callback>
			> options;*/
			Callback callback;

			constexpr explicit SystemSetup(Callback &&f): callback(f)
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
				return SystemSetup<F, Dependencies, Instance, Signature>(std::forward<F>(f));
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
